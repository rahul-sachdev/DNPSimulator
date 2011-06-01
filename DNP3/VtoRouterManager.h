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

#ifndef __VTO_ROUTER_MANAGER_H_
#define __VTO_ROUTER_MANAGER_H_

#include <map>

#include <APL/Types.h>
#include <APL/Loggable.h>
#include <APL/IPhysMonitor.h>

namespace apl {
	class ITimerSource;
	class IPhysicalLayerAsync;
}

namespace apl { namespace dnp {

class VtoRouter;
class VtoRouterSettings;
class IVtoWriter;

/** Class that automatically deletes itself and a router */
class RouterLifecycle : public IPhysMonitor
{
	public:
		RouterLifecycle(VtoRouter*);

		void OnStateChange(IPhysMonitor::State aState);
	
		VtoRouter* mpRouter;
};

class VtoRouterManager : private Loggable
{
public:
	VtoRouterManager(Logger* apLogger, ITimerSource* apTimerSrc);

	void StartRouter(
		const std::string& arStackName, 
		const VtoRouterSettings& arSettings, 
		IVtoWriter* apWriter, 
		IPhysicalLayerAsync* apPhys);

	void StopRouter(const std::string& arStackName, boost::uint8_t aChannelId);

	void StopAllRouters(const std::string& arStackName);

private:

	Logger* GetSubLogger(const std::string& arId, boost::uint8_t aVtoChannelId);
	
	typedef std::pair<std::string, boost::uint8_t> RouterKey;
	typedef std::map<RouterKey, RouterLifecycle*> RouterMap;	

	RouterMap mRouterMap;

	ITimerSource* mpTimerSrc;
};

}}

#endif
