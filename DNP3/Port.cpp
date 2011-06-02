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
#include "Port.h"

#include "Stack.h"
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include <APL/Logger.h>
#include <APL/IPhysicalLayerAsync.h>
#include <APL/AsyncTaskGroup.h>

namespace apl { namespace dnp {

Port::Port(const std::string& arName, Logger* apLogger, AsyncTaskGroup* apGroup, ITimerSource* apTimerSrc, IPhysicalLayerAsync* apPhys, millis_t aOpenDelay, IPhysMonitor* apObserver) :
Loggable(apLogger->GetSubLogger("port")),
CleanupHelper(apTimerSrc),
mName(arName),
mRouter(apLogger, apPhys, apTimerSrc, aOpenDelay),
mpGroup(apGroup),
mpPhys(apPhys),
mpObserver(apObserver),
mRelease(false)
{
	mRouter.SetMonitor(this);
}

// Releases the port now or when the router finall stops
void Port::Release()
{
	if(!mRelease)
	{
		mRelease = true;	
		if(!mRouter.IsRunning()) this->Cleanup();
	}
}

void Port::OnStateChange(IPhysMonitor::State aState)
{
	if(mpObserver != NULL) mpObserver->OnStateChange(aState);

	//when the router stops, delete ourselves
	if((aState == IPhysMonitor::Stopped) && mRelease) this->Cleanup();
}

void Port::Associate(const std::string& arStackName, Stack* apStack, const LinkRoute& arRoute)
{
	LOG_BLOCK(LEV_DEBUG, "Linking stack to port w/ route " << arRoute);
	mStackMap[arStackName] = StackRecord(apStack, arRoute);
	apStack->mLink.SetRouter(&mRouter);
	mRouter.AddContext(&apStack->mLink, arRoute);
	if(!mRouter.IsRunning()) {
		LOG_BLOCK(LEV_DEBUG, "Starting router");
		mRouter.Start();
	}
}

void Port::Disassociate(const std::string& arStackName)
{
	StackMap::iterator i = mStackMap.find(arStackName);
	StackRecord r = i->second;
	LOG_BLOCK(LEV_DEBUG, "Unlinking stack from port w/ route " << r.route);
	mRouter.RemoveContext(r.route);		// decouple the stack from the router and tell the stack to go offline if the it was previously online
	delete r.pStack;					// delete the stack
	if(mRouter.IsRunning() && mRouter.NumContext() == 0) {
		LOG_BLOCK(LEV_DEBUG, "Stopping router");
		mRouter.Stop();
	}
	mStackMap.erase(i);
}

}}
