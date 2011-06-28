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
#include "AsyncPhysLayerMonitor.h"

#include "IPhysicalLayerAsync.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <assert.h>
#include "Logger.h"

namespace apl
{

AsyncPhysLayerMonitor::AsyncPhysLayerMonitor(Logger* apLogger, IPhysicalLayerAsync* apPhys, ITimerSource* apTimerSrc, millis_t aOpenRetry) :
	Loggable(apLogger),
	IHandlerAsync(apLogger),
	mpPhys(apPhys),
	mpTimerSrc(apTimerSrc),
	mpOpenTimer(NULL),
	mPortState(apLogger, "port_state"),
	mOpening(false),
	mOpen(false),
	mStopOpenRetry(false),
	M_OPEN_RETRY(aOpenRetry),
	mpMonitor(NULL)
{
	assert(apPhys != NULL);
	assert(apTimerSrc != NULL);
	mpPhys->SetHandler(this);
}

AsyncPhysLayerMonitor::~AsyncPhysLayerMonitor()
{}

bool AsyncPhysLayerMonitor::IsRunning()
{
	return mOpen || mOpening || (mpOpenTimer != NULL);
}

bool AsyncPhysLayerMonitor::IsOpen()
{
	return mOpen;
}

void AsyncPhysLayerMonitor::SetMonitor(IPhysMonitor* apMonitor)
{
	mpMonitor = apMonitor;
}

void AsyncPhysLayerMonitor::Notify(PhysLayerState aState)
{
	LOG_BLOCK(LEV_DEBUG, "Transition to state: " << ConvertToString(aState));
	mPortState.Set(aState);
	this->OnStateChange(aState);
	if(mpMonitor) mpMonitor->OnStateChange(aState);
}

void AsyncPhysLayerMonitor::Start()
{
	LOG_BLOCK(LEV_DEBUG, "Start");
	assert(!mOpening);
	if(mpOpenTimer) mpOpenTimer = NULL;
	mOpening = true;
	mStopOpenRetry = false;
	mpPhys->AsyncOpen();
	this->Notify(PLS_OPENING);
}

void AsyncPhysLayerMonitor::Stop()
{
	LOG_BLOCK(LEV_DEBUG, "Stop");
	if(this->IsRunning()) {
		if(!mStopOpenRetry) {
			mStopOpenRetry = true;
			if(mOpen || mOpening) {
				mpPhys->AsyncClose();
			}
			if(mpOpenTimer) {
				mpOpenTimer->Cancel();
				mpOpenTimer = NULL;
			}
		}
	}
	else {
		mStopOpenRetry = true;
		this->Notify(PLS_STOPPED);
	}
}

void AsyncPhysLayerMonitor::Reconnect()
{
	if(mOpen || mOpening) {
		mpPhys->AsyncClose();
	}
}

void AsyncPhysLayerMonitor::_OnOpenFailure()
{
	mOpening = false;
	OnPhysicalLayerOpenFailure();
	if(mStopOpenRetry) {
		this->Notify(PLS_STOPPED); //we're done!
	}
	else {
		this->Notify(PLS_WAITING);
		mpOpenTimer = mpTimerSrc->Start(M_OPEN_RETRY, boost::bind(&AsyncPhysLayerMonitor::Start, this));
	}
}

void AsyncPhysLayerMonitor::_OnLowerLayerUp()
{
	assert(mOpening); mOpening = false; mOpen = true;
	this->OnPhysicalLayerOpen();
	this->Notify(PLS_OPEN);
}

void AsyncPhysLayerMonitor::_OnLowerLayerDown()
{
	mOpen = false;

	this->OnPhysicalLayerClose();
	this->Notify(PLS_CLOSED);

	if(mStopOpenRetry) {
		this->Notify(PLS_STOPPED);
	}
	else this->Start();
}

}
