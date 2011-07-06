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

#include "IOServicePauser.h"

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>

namespace apl
{

IOServicePauser::IOServicePauser(boost::asio::io_service* apService, size_t aNumThreads) :
	mpService(apService),
	mPausing(false),
	mNumThreads(aNumThreads),
	mPausedCount(0)
{}



void IOServicePauser::_Start()
{
	CriticalSection cs(&mLock);
	mPausing = true;
	mpService->post(boost::bind(&IOServicePauser::Pause, this));
	while(mPausedCount < mNumThreads) {
		mLock.Wait();
	}
}

void IOServicePauser::_End()
{
	CriticalSection cs(&mLock);
	mPausing = false;
	cs.Broadcast();
}

void IOServicePauser::Pause()
{
	CriticalSection cs(&mLock);
	++mPausedCount;
	cs.Broadcast();
	while(mPausing) cs.Wait();
	--mPausedCount;
}

}

/* vim: set ts=4 sw=4: */


