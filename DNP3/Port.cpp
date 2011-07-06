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

namespace apl
{
namespace dnp
{

Port::Port(const std::string& arName, Logger* apLogger, ITimerSource* apTimerSrc, IPhysicalLayerAsync* apPhys, millis_t aOpenDelay) :
	Loggable(apLogger->GetSubLogger("port")),
	mName(arName),
	mRouter(apLogger, apPhys, apTimerSrc, aOpenDelay),
	mpPhys(apPhys),
	mState(PLS_CLOSED)
{
	mRouter.AddMonitor(this);
}

void Port::OnStateChange(PhysLayerState aState)
{
	CriticalSection cs(&mLock);
	mState = aState;
	cs.Signal();
}

void Port::WaitForStop()
{
	CriticalSection cs(&mLock);
	while(mState != PLS_STOPPED) cs.Wait();
}

void Port::BindStackToPort(const std::string& arStackName, Stack* apStack, const LinkRoute& arRoute)
{
	LOG_BLOCK(LEV_DEBUG, "Linking stack to port w/ route " << arRoute);
	mRouter.AddContext(&apStack->mLink, arRoute); // this function can throw, do it first
	apStack->mLink.SetRouter(&mRouter);
	mStackMap[arStackName] = StackRecord(apStack, arRoute);
	mRouter.Start();
}

void Port::RemoveStackFromPort(const std::string& arStackName)
{
	StackMap::iterator i = mStackMap.find(arStackName);
	if(i == mStackMap.end()) throw ArgumentException("Stack with that name not bound: " + arStackName);
	else {
		StackRecord r = i->second;
		LOG_BLOCK(LEV_DEBUG, "Unlinking stack from port w/ route " << r.route);
		mRouter.RemoveContext(r.route);
		mStackMap.erase(i);
		if(mRouter.NumContext() == 0) {
			LOG_BLOCK(LEV_DEBUG, "Stopping router");
			mRouter.Stop();
		}
	}
}

}
}
