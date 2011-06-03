/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership.  Green Enery
 * Corp licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "VtoRouterManager.h"

#include "VtoRouter.h"
#include "VtoRouterSettings.h"

#include <APL/Exception.h>
#include <APL/IPhysicalLayerSource.h>
#include <APL/IPhysicalLayerAsync.h>

#include <boost/foreach.hpp>
#include <sstream>

namespace apl { namespace dnp {

VtoRouterManager::RouterRecord::RouterRecord(const std::string& arPortName, VtoRouter* apRouter, IVtoWriter* apWriter, boost::uint8_t aVtoChannelId) :
	mPortName(arPortName),
	mpRouter(apRouter),
	mpWriter(apWriter),
	mVtoChannelId(aVtoChannelId)
{
	
}

VtoRouterManager::VtoRouterManager(Logger* apLogger, ITimerSource* apTimerSrc, IPhysicalLayerSource* apPhysSrc) :
	Loggable(apLogger),
	mpTimerSrc(apTimerSrc),
	mpPhysSource(apPhysSrc)	
{	
	assert(apTimerSrc != NULL);	
	assert(apPhysSrc != NULL);	
}

VtoRouterManager::~VtoRouterManager()
{
	this->StopAllRouters();
}

void VtoRouterManager::ClenupAfterRouter(IPhysicalLayerAsync* apPhys, VtoRouter* apRouter)
{
	delete apPhys;
	delete apRouter;
}

VtoRouter* VtoRouterManager::StartRouter(		
		const std::string& arPortName,
		const VtoRouterSettings& arSettings, 
		IVtoWriter* apWriter)
{		
	IPhysicalLayerAsync* pPhys = mpPhysSource->AcquireLayer(arPortName, false); //don't autodelete
	Logger* pLogger = this->GetSubLogger(arPortName, arSettings.CHANNEL_ID);
				
	VtoRouter* pRouter = new VtoRouter(arSettings, pLogger, apWriter, pPhys, mpTimerSrc);

	// when the router is completely stopped, it's physical layer will be deleted, followed by itself
	pRouter->AddCleanupTask(boost::bind(&VtoRouterManager::ClenupAfterRouter, pPhys, pRouter));

	RouterRecord record(arPortName, pRouter, apWriter, arSettings.CHANNEL_ID);

	pRouter->Start();
	
	this->mRecords.push_back(record);

	return pRouter;
}

std::vector<VtoRouterManager::RouterRecord> VtoRouterManager::GetAllRouters()
{
	std::vector<VtoRouterManager::RouterRecord> ret;
	for(size_t i=0; i<mRecords.size(); ++i) ret.push_back(mRecords[i]);
	return ret;
}

void VtoRouterManager::StopRouters(const std::vector<RouterRecord>& arRouters)
{
	for(size_t i=0; i< arRouters.size(); ++i) this->StopRouter(arRouters[i].mpRouter);
}
	
void VtoRouterManager::StopAllRouters()
{
	std::vector<VtoRouterManager::RouterRecord> routers = this->GetAllRouters();
	this->StopRouters(routers);
}

void VtoRouterManager::StopRouter(IVtoWriter* apWriter, boost::uint8_t aVtoChannelId)
{
	this->StopRouter(this->GetRouterOnWriter(apWriter, aVtoChannelId).mpRouter);
}

void VtoRouterManager::StopAllRoutersOnWriter(IVtoWriter* apWriter)
{
	std::vector<RouterRecord> routers = this->GetAllRoutersOnWriter(apWriter);
	this->StopRouters(routers);
}

std::vector<VtoRouterManager::RouterRecord> VtoRouterManager::GetAllRoutersOnWriter(IVtoWriter* apWriter)
{
	std::vector< VtoRouterManager::RouterRecord > ret;

	for(RouterRecordVector::iterator i = this->mRecords.begin(); i != mRecords.end(); ++i) {
		if(i->mpWriter == apWriter) ret.push_back(*i);
	}
	
	return ret;
}

VtoRouterManager::RouterRecord VtoRouterManager::GetRouterOnWriter(IVtoWriter* apWriter, boost::uint8_t aVtoChannelId)
{
	for(RouterRecordVector::iterator i = this->mRecords.begin(); i != mRecords.end(); ++i) {
		if(i->mpWriter == apWriter && i->mVtoChannelId == aVtoChannelId) return *i;
	}	

	throw ArgumentException(LOCATION, "Router not found for writer on channel");
}


VtoRouterManager::RouterRecordVector::iterator VtoRouterManager::Find(IVtoWriter* apWriter, boost::uint8_t aVtoChannelId)
{
	RouterRecordVector::iterator i = this->mRecords.begin();

	for(; i != mRecords.end(); ++i) {
		if(i->mpWriter == apWriter && i->mVtoChannelId == aVtoChannelId) return i;
	}

	return i;
}

VtoRouterManager::RouterRecordVector::iterator VtoRouterManager::Find(IVtoWriter* apWriter)
{
	RouterRecordVector::iterator i = this->mRecords.begin();

	for(; i != mRecords.end(); ++i) {
		if(i->mpWriter == apWriter) return i;
	}

	return i;
}

void VtoRouterManager::StopRouter(VtoRouter* apRouter)
{	
	for(RouterRecordVector::iterator i = mRecords.begin(); i != mRecords.end(); ++i) 
	{
		if(i->mpRouter == apRouter) {
			mpPhysSource->ReleaseLayer(i->mPortName);
			mpTimerSrc->Post(boost::bind(&VtoRouter::Stop, i->mpRouter));
			mRecords.erase(i);
			return;
		}
	}
	
	throw ArgumentException(LOCATION, "Router could not be found in vector");
}

Logger* VtoRouterManager::GetSubLogger(const std::string& arId, boost::uint8_t aVtoChannelId)
{
	std::ostringstream oss;
	oss << arId << "-VtoRouterChannel-" << aVtoChannelId;
	return mpLogger->GetSubLogger(oss.str());	
}

}}
