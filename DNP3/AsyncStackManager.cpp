//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  Green Enery Corp licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//

#include "AsyncStackManager.h"
#include "LinkChannel.h"

#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include <APL/TimerSourceASIO.h>
#include <APL/Exception.h>
#include <APL/Logger.h>
#include <APL/IPhysicalLayerAsync.h>

#include <DNP3/MasterStack.h>
#include <DNP3/SlaveStack.h>
#include <DNP3/DeviceTemplate.h>
#include <DNP3/VtoRouter.h>
#include <DNP3/VtoConfig.h>

#include <iostream>


using namespace std;

namespace apl
{
namespace dnp
{

template <class T, class U>
static std::vector<U> GetKeys(T& arMap)
{
	std::vector<U> ret;
	for(typename T::iterator i = arMap.begin(); i != arMap.end(); ++i) {
		ret.push_back(i->first);
	}
	return ret;
}

AsyncStackManager::AsyncStackManager(Logger* apLogger) :
	Loggable(apLogger),		
	mService(),
	mTimerSrc(mService.Get()),	
	mIOServicePauser(mService.Get(), 1),
	mMgr(apLogger->GetSubLogger("channels", LEV_WARNING), mService.Get()),
	mScheduler(&mTimerSrc),
	mVtoManager(apLogger->GetSubLogger("vto"), &mTimerSrc, &mMgr),
	mThread(this),
	mpInfiniteTimer(mTimerSrc.StartInfinite(boost::bind(&AsyncStackManager::NullActionForInfiniteTimer)))
{	
	mThread.Start();
}

AsyncStackManager::~AsyncStackManager()
{
	/*
	 * Tell every channel to stop
	 */
	this->Shutdown();

	// if we've cleaned up correctly, canceling the infinite timer will cause the thread to stop executing
	mpInfiniteTimer->Cancel();
	LOG_BLOCK(LEV_DEBUG, "Joining on io_service thread");
	mThread.WaitForStop();
	LOG_BLOCK(LEV_DEBUG, "Join complete on io_service thread");
}

std::vector<std::string> AsyncStackManager::GetStackNames()
{
	return GetKeys<StackToChannelMap, string>(mStackNameToChannel);
}

std::vector<std::string> AsyncStackManager::GetPortNames()
{
	return GetKeys<ChannelToChannelMap, string>(mChannelNameToChannel);
}

void AsyncStackManager::AddTCPClient(const std::string& arName, PhysLayerSettings aSettings, const std::string& arAddr, boost::uint16_t aPort)
{
	mMgr.AddTCPClient(arName, aSettings, arAddr, aPort);
}

void AsyncStackManager::AddTCPServer(const std::string& arName, PhysLayerSettings aSettings, const std::string& arEndpoint, boost::uint16_t aPort)
{
	mMgr.AddTCPServer(arName, aSettings, arEndpoint, aPort);
}

void AsyncStackManager::AddSerial(const std::string& arName, PhysLayerSettings aSettings, SerialSettings aSerial)
{
	mMgr.AddSerial(arName, aSettings, aSerial);
}

ICommandAcceptor* AsyncStackManager::AddMaster( const std::string& arPortName, const std::string& arStackName, FilterLevel aLevel, IDataObserver* apPublisher,
        const MasterStackConfig& arCfg)
{
	LinkChannel* pChannel = this->GetOrCreateChannel(arPortName);
	Logger* pLogger = mpLogger->GetSubLogger(arStackName, aLevel);
	pLogger->SetVarName(arStackName);

	boost::shared_ptr<MasterStack> pMaster(new MasterStack(pLogger, &mTimerSrc, apPublisher, pChannel->GetGroup(), arCfg));
	LinkRoute route(arCfg.link.RemoteAddr, arCfg.link.LocalAddr);

	this->OnAddStack(arStackName, pMaster, pChannel, route);

	// add any vto routers we've configured
	BOOST_FOREACH(VtoRouterConfig s, arCfg.vto.mRouterConfigs) {
		this->StartVtoRouter(s.mPhysicalLayerName, arStackName, s.mSettings);
	}

	return pMaster->mMaster.GetCmdAcceptor();
}

IDataObserver* AsyncStackManager::AddSlave( const std::string& arPortName, const std::string& arStackName, FilterLevel aLevel, ICommandAcceptor* apCmdAcceptor,
        const SlaveStackConfig& arCfg)
{
	LinkChannel* pChannel = this->GetOrCreateChannel(arPortName);
	Logger* pLogger = mpLogger->GetSubLogger(arStackName, aLevel);
	pLogger->SetVarName(arStackName);

	boost::shared_ptr<SlaveStack> pSlave(new SlaveStack(pLogger, &mTimerSrc, apCmdAcceptor, arCfg));
	
	LinkRoute route(arCfg.link.RemoteAddr, arCfg.link.LocalAddr);
	this->OnAddStack(arStackName, pSlave, pChannel, route);

	// add any vto routers we've configured
	BOOST_FOREACH(VtoRouterConfig s, arCfg.vto.mRouterConfigs) {
		this->StartVtoRouter(s.mPhysicalLayerName, arStackName, s.mSettings);
	}

	return pSlave->mSlave.GetDataObserver();
}

void AsyncStackManager::AddVtoChannel(const std::string& arStackName,
                                      IVtoCallbacks* apCallbacks)
{
	Stack* pStack = this->GetStackByName(arStackName);
	pStack->GetVtoWriter()->AddVtoCallback(apCallbacks);
	pStack->GetVtoReader()->AddVtoChannel(apCallbacks);
}

void AsyncStackManager::RemoveVtoChannel(const std::string& arStackName, IVtoCallbacks* apCallbacks)
{
	Stack* pStack = this->GetStackByName(arStackName);
	pStack->GetVtoWriter()->RemoveVtoCallback(apCallbacks);
	pStack->GetVtoReader()->RemoveVtoChannel(apCallbacks);
}

void AsyncStackManager::StartVtoRouter(const std::string& arPortName,
                                       const std::string& arStackName, const VtoRouterSettings& arSettings)
{
	Stack* pStack = this->GetStackByName(arStackName);
	VtoRouter* pRouter = mVtoManager.StartRouter(arPortName, arSettings, pStack->GetVtoWriter());
	this->AddVtoChannel(arStackName, pRouter);
}

void AsyncStackManager::StopVtoRouter(const std::string& arStackName, boost::uint8_t aVtoChannelId)
{
	Stack* pStack = this->GetStackByName(arStackName);
	IVtoWriter* pWriter = pStack->GetVtoWriter();
	VtoRouterManager::RouterRecord rec = mVtoManager.GetRouterOnWriter(pWriter, aVtoChannelId);
	this->RemoveVtoChannel(arStackName, rec.mpRouter);
	mVtoManager.StopRouter(pWriter, aVtoChannelId);
}

void AsyncStackManager::StopAllRoutersOnStack(const std::string& arStackName)
{
	IVtoWriter* pWriter = this->GetVtoWriter(arStackName);
	//mVtoManager.StopAllRoutersOnWriter(pWriter);
	//TODO - figure out why this is commented out
}

IVtoWriter* AsyncStackManager::GetVtoWriter(const std::string& arStackName)
{
	return this->GetStackByName(arStackName)->GetVtoWriter();
}

// Remove a port and all associated stacks
void AsyncStackManager::RemovePort(const std::string& arPortName)
{
	LinkChannel* pChannel = this->GetChannelOrExcept(arPortName);
	vector<string> stacks = this->StacksOnChannel(arPortName);
	BOOST_FOREACH(string s, stacks) { this->SeverStack(pChannel, s); }

	this->mScheduler.ReleaseGroup(pChannel->GetGroup());
	
	pChannel->WaitForStop();
	mChannelNameToChannel.erase(arPortName);

	// remove the physical layer from the list	
	mMgr.Remove(arPortName);	
}

std::vector<std::string> AsyncStackManager::StacksOnChannel(const std::string& arPortName)
{
	std::vector<std::string> ret;
	for(StackToChannelMap::iterator i = this->mStackNameToChannel.begin(); i != mStackNameToChannel.end(); ++i) {
		if(i->second->Name() == arPortName) {
			ret.push_back(i->first);
		}
	}
	return ret;
}

void AsyncStackManager::RemoveStack(const std::string& arStackName)
{
	LinkChannel* pChannel = this->GetChannelByStackName(arStackName);
	this->SeverStack(pChannel, arStackName);	
}

void AsyncStackManager::SeverStack(LinkChannel* apChannel, const std::string& arStackName)
{	
	/*
	IVtoWriter* pWriter = this->GetStackByName(arStackName)->GetVtoWriter();
	std::vector<VtoRouterManager::RouterRecord> records = mVtoManager.GetAllRoutersOnWriter(pWriter);
	BOOST_FOREACH(VtoRouterManager::RouterRecord rec, records) {
		this->RemoveVtoChannel(arStackName, rec.mpRouter);
		mVtoManager.StopRouter(pWriter, rec.mVtoChannelId);
	}
	*/

	// when removing a stack, we need to pause execution
	{
		Transaction tr(&mIOServicePauser);
		apChannel->RemoveStackFromChannel(arStackName);
	}	

	mStackNameToChannel.erase(arStackName);
	mStackNameToStack.erase(arStackName);
}

LinkChannel* AsyncStackManager::GetChannelByStackName(const std::string& arStackName)
{
	StackToChannelMap::iterator i = mStackNameToChannel.find(arStackName);
	if(i == mStackNameToChannel.end()) throw ArgumentException(LOCATION, "Unknown stack: " + arStackName);
	return i->second;
}

Stack* AsyncStackManager::GetStackByName(const std::string& arStackName)
{
	StackMap::iterator i = mStackNameToStack.find(arStackName);
	if (i == mStackNameToStack.end()) {
		throw ArgumentException(LOCATION, "Unknown stack");
	}

	return i->second.get();
}

void AsyncStackManager::Shutdown()
{	
	vector<string> ports = this->GetPortNames();
	BOOST_FOREACH(string s, ports) {
		LOG_BLOCK(LEV_INFO, "Removing port: " << s);
		this->RemovePort(s);
		LOG_BLOCK(LEV_INFO, "Done removing Port: " << s);
	}		
}

LinkChannel* AsyncStackManager::GetOrCreateChannel(const std::string& arName)
{
	LinkChannel* pChannel = this->GetChannelMaybeNull(arName);
	return (pChannel == NULL) ? this->CreateChannel(arName) : pChannel; 	
}

LinkChannel* AsyncStackManager::GetChannelOrExcept(const std::string& arName)
{
	LinkChannel* pChannel = this->GetChannelMaybeNull(arName);
	if(pChannel == NULL) throw ArgumentException(LOCATION, "Channel doesn't exist: " + arName);
	return pChannel;
}

LinkChannel* AsyncStackManager::CreateChannel(const std::string& arName)
{
	if(GetChannelMaybeNull(arName) != NULL) throw ArgumentException(LOCATION, "Channel already exists with name: " + arName);

	PhysLayerSettings s = mMgr.GetSettings(arName);
	IPhysicalLayerAsync* pPhys = mMgr.AcquireLayer(arName);
	Logger* pChannelLogger = mpLogger->GetSubLogger(arName, s.LogLevel);
	pChannelLogger->SetVarName(arName);
	AsyncTaskGroup* pGroup = mScheduler.CreateNewGroup();

	boost::shared_ptr<LinkChannel> pChannel(new LinkChannel(pChannelLogger, arName, &mTimerSrc, pPhys, pGroup, s.RetryTimeout));	
	mChannelNameToChannel[arName] = pChannel;
	return pChannel.get();
}

LinkChannel* AsyncStackManager::GetChannelMaybeNull(const std::string& arName)
{
	ChannelToChannelMap::iterator i = mChannelNameToChannel.find(arName);
	return (i == mChannelNameToChannel.end()) ? NULL : i->second.get();
}

void AsyncStackManager::Run()
{
	size_t num = 0;

	do {
		try {
			num = mService.Get()->run();
		}
		catch(const std::exception& ex) {
			LOG_BLOCK(LEV_ERROR, "Unhandled exception: " << ex.what());
		}
	}
	while(num > 0);

	mService.Get()->reset();
}

void AsyncStackManager::OnAddStack(const std::string& arStackName, boost::shared_ptr<Stack> apStack, LinkChannel* apChannel, const LinkRoute& arRoute)
{
	
	{	// when binding the stack to the router, we need to pause excution
		Transaction tr(&mIOServicePauser);
		apChannel->BindStackToChannel(arStackName, apStack.get(), arRoute);		
	}
	
	mStackNameToChannel[arStackName] = apChannel;	// map the stack name to a LinkChannel object
	mStackNameToStack[arStackName] = apStack;		// map the stack name to a Stack object		
}

}
}

/* vim: set ts=4 sw=4: */
