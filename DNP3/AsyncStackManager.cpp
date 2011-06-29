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
#include "Port.h"

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

AsyncStackManager::AsyncStackManager(Logger* apLogger, bool aAutoRun) :
	Loggable(apLogger),
	mRunASIO(aAutoRun),
	mRunning(false),
	mService(),
	mTimerSrc(mService.Get()),
	mMgr(apLogger->GetSubLogger("ports", LEV_WARNING), mService.Get()),
	mScheduler(&mTimerSrc),
	mVtoManager(apLogger->GetSubLogger("VtoRouterManager"), &mTimerSrc, &mMgr),
	mThread(this)
{}

AsyncStackManager::~AsyncStackManager()
{
	/*
	 * Tell every port to stop, join on the thread.
	 */
	this->Stop();

}

std::vector<std::string> AsyncStackManager::GetStackNames()
{
	return GetKeys<PortMap, string>(mStackNameToPort);
}

std::vector<std::string> AsyncStackManager::GetPortNames()
{
	return GetKeys<PortMap, string>(mPortNameToPort);
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
	Port* pPort = this->AllocatePort(arPortName);
	Logger* pLogger = mpLogger->GetSubLogger(arStackName, aLevel);
	pLogger->SetVarName(arStackName);
	MasterStack* pMaster = new MasterStack(pLogger, &mTimerSrc, apPublisher, pPort->GetGroup(), arCfg);
	LinkRoute route(arCfg.link.RemoteAddr, arCfg.link.LocalAddr);
	this->OnAddStack(arStackName, pMaster, pPort, route);

	// add any vto routers we've configured
	BOOST_FOREACH(VtoRouterConfig s, arCfg.vto.mRouterConfigs) {
		this->StartVtoRouter(s.mPhysicalLayerName, arStackName, s.mSettings);
	}

	return pMaster->mMaster.GetCmdAcceptor();
}

IDataObserver* AsyncStackManager::AddSlave( const std::string& arPortName, const std::string& arStackName, FilterLevel aLevel, ICommandAcceptor* apCmdAcceptor,
        const SlaveStackConfig& arCfg)
{
	Port* pPort = this->AllocatePort(arPortName);
	Logger* pLogger = mpLogger->GetSubLogger(arStackName, aLevel);
	pLogger->SetVarName(arStackName);
	SlaveStack* pSlave = new SlaveStack(pLogger, &mTimerSrc, apCmdAcceptor, arCfg);
	LinkRoute route(arCfg.link.RemoteAddr, arCfg.link.LocalAddr);
	this->OnAddStack(arStackName, pSlave, pPort, route);

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
	Port* pPort = this->GetPort(arPortName);
	vector<string> stacks = this->StacksOnPort(arPortName);
	BOOST_FOREACH(string s, stacks) {
		this->SeverStack(pPort, s);
	}
	mPortNameToPort.erase(arPortName);

	// this will cause the port to be released on the io_service thread
	mTimerSrc.Post(boost::bind(&Port::Release, pPort));

	// remove the physical layer from the list
	// The ports own the layers themselves, so deleting the port will delete the layer
	mMgr.Remove(arPortName);

	this->CheckForJoin();
}

std::vector<std::string> AsyncStackManager::StacksOnPort(const std::string& arPortName)
{
	std::vector<std::string> ret;
	for(PortMap::iterator i = this->mStackNameToPort.begin(); i != mStackNameToPort.end(); ++i) {
		if(i->second->Name() == arPortName) ret.push_back(i->first);
	}
	return ret;
}

void AsyncStackManager::RemoveStack(const std::string& arStackName)
{
	Port* pPort = this->GetPortByStackName(arStackName);
	this->SeverStack(pPort, arStackName);
	this->CheckForJoin();
}

void AsyncStackManager::SeverStack(Port* apPort, const std::string& arStackName)
{
	IVtoWriter* pWriter = this->GetStackByName(arStackName)->GetVtoWriter();
	std::vector<VtoRouterManager::RouterRecord> records = mVtoManager.GetAllRoutersOnWriter(pWriter);
	BOOST_FOREACH(VtoRouterManager::RouterRecord rec, records) {
		this->RemoveVtoChannel(arStackName, rec.mpRouter);
		mVtoManager.StopRouter(pWriter, rec.mVtoChannelId);
	}
	//this will cause the stack to be pulled off the port and deleted on the io_service thread
	mTimerSrc.Post(boost::bind(&Port::Disassociate, apPort, arStackName));
	mStackNameToPort.erase(arStackName);
	mStackNameToStack.erase(arStackName);
}

Port* AsyncStackManager::GetPortByStackName(const std::string& arStackName)
{
	PortMap::iterator i = mStackNameToPort.find(arStackName);
	if(i == mStackNameToPort.end()) throw ArgumentException(LOCATION, "Unknown stack");
	return i->second;
}

Stack* AsyncStackManager::GetStackByName(const std::string& arStackName)
{
	StackMap::iterator i = mStackNameToStack.find(arStackName);
	if (i == mStackNameToStack.end()) {
		throw ArgumentException(LOCATION, "Unknown stack");
	}

	return i->second;
}

void AsyncStackManager::Stop()
{
	LOG_BLOCK(LEV_DEBUG, "enter stop");
	vector<string> ports = this->GetPortNames();
	BOOST_FOREACH(string s, ports) {
		LOG_BLOCK(LEV_DEBUG, "Removing port: " << s);
		this->RemovePort(s);
		LOG_BLOCK(LEV_DEBUG, "Done removing Port: " << s);
	}
	if(mRunning) {
		LOG_BLOCK(LEV_DEBUG, "Joining on io_service thread");
		mThread.WaitForStop();
		mRunning = false;
		LOG_BLOCK(LEV_INFO, "Done joining");
	}

	/*
	 * Run out the io_service to verify that all the delete operations are
	 * called, just in case the io_service wasn't running before.
	 */
	LOG_BLOCK(LEV_DEBUG, "extra run");
	this->Run();

	LOG_BLOCK(LEV_DEBUG, "exit stop");
}

void AsyncStackManager::Start()
{
	if(this->NumStacks() > 0 && !mRunning) {
		mRunning = true;
		mThread.Start();
	}
}

Port* AsyncStackManager::AllocatePort(const std::string& arName)
{
	Port* pPort = this->GetPortPointer(arName);
	if(pPort == NULL) return this->CreatePort(arName);
	else return pPort;
}

Port* AsyncStackManager::GetPort(const std::string& arName)
{
	Port* pPort = this->GetPortPointer(arName);
	if(pPort == NULL) throw ArgumentException(LOCATION, "Port doesn't exist");
	return pPort;
}

void AsyncStackManager::DeletePort(Port* apPort)
{
	delete apPort;
}

void AsyncStackManager::DeleteLayer(IPhysicalLayerAsync* apLayer)
{
	delete apLayer;
}

Port* AsyncStackManager::CreatePort(const std::string& arName)
{
	if(GetPortPointer(arName) != NULL) throw ArgumentException(LOCATION, "Port already exists");

	PhysLayerSettings s = mMgr.GetSettings(arName);
	IPhysicalLayerAsync* pPhys = mMgr.AcquireLayer(arName, false); //important! We'll delete the pointer manually
	Logger* pPortLogger = mpLogger->GetSubLogger(arName, s.LogLevel);
	pPortLogger->SetVarName(arName);

	AsyncTaskGroup* pGroup = mScheduler.CreateNewGroup();

	Port* pPort = new Port(arName, pPortLogger, pGroup, &mTimerSrc, pPhys, s.RetryTimeout, s.mpObserver);

	// when the port is completely terminated, release these resources
	pPort->AddCleanupTask(boost::bind(&AsyncTaskScheduler::ReleaseGroup, &mScheduler, pGroup));
	pPort->AddCleanupTask(boost::bind(&AsyncStackManager::DeletePort, pPort));
	pPort->AddCleanupTask(boost::bind(&AsyncStackManager::DeleteLayer, pPhys));

	mPortNameToPort[arName] = pPort;
	return pPort;
}

Port* AsyncStackManager::GetPortPointer(const std::string& arName)
{
	PortMap::iterator i = mPortNameToPort.find(arName);
	return (i == mPortNameToPort.end()) ? NULL : i->second;
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

void AsyncStackManager::OnAddStack(const std::string& arStackName, Stack* apStack, Port* apPort, const LinkRoute& arRoute)
{
	// marshall the linking to the io_service
	mStackNameToPort[arStackName] = apPort; // map the stack name to a Port object
	mStackNameToStack[arStackName] = apStack;	// map the stack name to a Stack object

	mTimerSrc.Post(boost::bind(&Port::Associate, apPort, arStackName, apStack, arRoute));
	if(!mRunning && mRunASIO) {
		mRunning = true;
		mThread.Start();
	}
}

void AsyncStackManager::CheckForJoin()
{
	if(mRunning && this->NumStacks() == 0) {
		LOG_BLOCK(LEV_DEBUG, "Check For join: joining on io_service thread");
		mThread.WaitForStop();	//join on the thread, ASIO will exit when there's no more work to be done
		mRunning = false;
		LOG_BLOCK(LEV_DEBUG, "Check For join: complete");
	}
}

}
}

/* vim: set ts=4 sw=4: */
