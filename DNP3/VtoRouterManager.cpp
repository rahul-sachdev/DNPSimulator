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

#include <boost/foreach.hpp>
#include <sstream>

namespace apl { namespace dnp {

RouterLifecycle::RouterLifecycle(VtoRouter* apRouter) : mpRouter(apRouter)
{
	assert(apRouter != NULL);
	apRouter->SetMonitor(this);
}

void RouterLifecycle::OnStateChange(IPhysMonitor::State aState)
{
	assert(mpRouter != NULL);

	if(aState == IPhysMonitor::Stopped) {
		delete mpRouter;
		mpRouter = NULL;
		delete this;
	}
}

VtoRouterManager::VtoRouterManager(Logger* apLogger, ITimerSource* apTimerSrc) :
	Loggable(apLogger),
	mpTimerSrc(apTimerSrc)
{
	assert(apTimerSrc != NULL);	
}

void VtoRouterManager::StartRouter(
		const std::string& arStackName, 
		const VtoRouterSettings& arSettings, 
		IVtoWriter* apWriter, 
		IPhysicalLayerAsync* apPhys)
{
	RouterKey key(arStackName, arSettings.CHANNEL_ID);
	RouterMap::iterator i = mRouterMap.find(key);
	if(i == mRouterMap.end())
	{	
		Logger* pLogger = this->GetSubLogger(arStackName, arSettings.CHANNEL_ID); 
		VtoRouter* pRouter = new VtoRouter(arSettings, pLogger, apWriter, apPhys, mpTimerSrc);
		RouterLifecycle* pLifecycle = new RouterLifecycle(pRouter);
		mRouterMap.insert(RouterMap::value_type(key, pLifecycle));
	}
	else
	{
		std::ostringstream oss;
		oss << "Name: " << arStackName << " ChannelId: " << arSettings.CHANNEL_ID << " already bound to a router";
		throw ArgumentException(LOCATION, oss.str());
	}
}

void VtoRouterManager::StopAllRouters(const std::string& arStackName)
{
	std::vector<RouterKey> keysToRemove;

	for(RouterMap::iterator i = mRouterMap.begin(); i != mRouterMap.end(); ++i)
	{
		if(i->first.first == arStackName) keysToRemove.push_back(i->first);
	}

	BOOST_FOREACH(RouterKey key, keysToRemove)
	{
		this->StopRouter(key.first, key.second);
	}
}

void VtoRouterManager::StopRouter(const std::string& arStackName, boost::uint8_t aChannelId)
{
	RouterMap::iterator i = mRouterMap.find(RouterKey(arStackName, aChannelId)); 
	if(i == mRouterMap.end())
	{
		std::ostringstream oss;
		oss << "Name: " << arStackName << " ChannelId: " << aChannelId << " not registered";
		throw ArgumentException(LOCATION, oss.str());
	}
	else
	{
		// Shutdown the router from another thread. It will automatically delete itself when it stops.
		mpTimerSrc->Post(boost::bind(&VtoRouter::Stop, i->second->mpRouter));
		mRouterMap.erase(i);
	}
}

Logger* VtoRouterManager::GetSubLogger(const std::string& arId, boost::uint8_t aVtoChannelId)
{
	std::ostringstream oss;
	oss << "VtoRouterChannel-" << aVtoChannelId;
	return mpLogger->GetSubLogger(oss.str());	
}

}}
