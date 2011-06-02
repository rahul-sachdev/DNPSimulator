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

#include <boost/bind.hpp>
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

VtoRouterManager::VtoRouterManager(Logger* apLogger, ITimerSource* apTimerSrc, IPhysicalLayerSource* apPhysSrc, boost::asio::io_service* apService) :
	Loggable(apLogger),
	mpTimerSrc(apTimerSrc),
	mpPhysSource(apPhysSrc),
	mpService(apService)
{
	assert(apTimerSrc != NULL);
	assert(apPhysSrc != NULL);
	assert(apService != NULL);
}

void VtoRouterManager::ClenupAfterRouter(IPhysicalLayerAsync* apPhys, VtoRouter* apRouter)
{
	delete apPhys;
	delete apRouter;
}

void VtoRouterManager::StartRouter(
		const std::string& arPortName,
		const VtoRouterSettings& arSettings,
		IVtoWriter* apWriter)
{
	IPhysicalLayerAsync* pPhys = mpPhysSource->AcquireLayer(arPortName, mpService, false); //don't autodelete
	Logger* pLogger = this->GetSubLogger(arPortName, arSettings.CHANNEL_ID);

	VtoRouter* pRouter = new VtoRouter(arSettings, pLogger, apWriter, pPhys, mpTimerSrc);

	// when the router is completely stopped, it's physical layer will be deleted, followed by itself
	pRouter->AddCleanupTask(boost::bind(&VtoRouterManager::ClenupAfterRouter, pPhys, pRouter));

	RouterRecord record(arPortName, pRouter, apWriter, arSettings.CHANNEL_ID);

	pRouter->Start();

	this->mRecords.push_back(record);
}

void VtoRouterManager::StopRouterOnWriter(IVtoWriter* apWriter, boost::uint8_t aVtoChannelId)
{
	RouterRecordVector::iterator i = this->Find(apWriter, aVtoChannelId);
	if(i != mRecords.end()) {
		this->StopRouter(i);
		this->StopRouterOnWriter(apWriter, aVtoChannelId);
	}
}

void VtoRouterManager::StopAllRouters()
{
	if(this->mRecords.size() != 0) {
		this->StopRouter(mRecords.begin());
		this->StopAllRouters();
	}
}

void VtoRouterManager::StopAllRoutersOnWriter(IVtoWriter* apWriter)
{
	RouterRecordVector::iterator i = this->Find(apWriter);
	if(i != mRecords.end()) {
		this->StopRouter(i);
		this->StopAllRoutersOnWriter(apWriter);
	}
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

void VtoRouterManager::StopRouter(const RouterRecordVector::iterator& arIter)
{
	mpPhysSource->ReleaseLayer(arIter->mPortName);
	// Shutdown the router from another thread. It will automatically clean up after it stops
	mpTimerSrc->Post(boost::bind(&VtoRouter::Stop, arIter->mpRouter));
	this->mRecords.erase(arIter);
}

Logger* VtoRouterManager::GetSubLogger(const std::string& arId, boost::uint8_t aVtoChannelId)
{
	std::ostringstream oss;
	oss << arId << "-VtoRouterChannel-" << aVtoChannelId;
	return mpLogger->GetSubLogger(oss.str());
}

}}
