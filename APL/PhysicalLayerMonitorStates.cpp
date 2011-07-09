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

#include "PhysicalLayerMonitorStates.h"

#include "Logger.h"
#include "Exception.h"
#include "PhysicalLayerMonitor.h"
#include "IPhysicalLayerAsync.h"

namespace apl
{

/* --- NotOpening --- */

void NotOpening::OnLayerOpen(PhysicalLayerMonitor* apContext)
{
	throw InvalidStateException(LOCATION, "Not opening: " + this->Name());
}

void NotOpening::OnOpenFailure(PhysicalLayerMonitor* apContext)
{
	throw InvalidStateException(LOCATION, "Not opening: " + this->Name());
}

/* --- NotOpen --- */

void NotOpen::OnLayerClose(PhysicalLayerMonitor* apContext)
{
	throw InvalidStateException(LOCATION, "Not open: " + this->Name());
}

/* --- NotWaitingForTimer --- */

void NotWaitingForTimer::OnOpenTimeout(PhysicalLayerMonitor* apContext)
{
	throw InvalidStateException(LOCATION, "Not waiting for timer: " + this->Name());
}

/* --- IgnoresClose --- */

void IgnoresClose::OnCloseRequest(PhysicalLayerMonitor* apContext)
{
	LOGGER_BLOCK(apContext->mpLogger, LEV_INFO, "Ignoring Close(): " << this->Name());
}

/* --- HandlesClose --- */

void HandlesClose::OnLayerClose(PhysicalLayerMonitor* apContext)
{
	apContext->mpPhys->AsyncOpen();
	apContext->ChangeState(MonitorStateOpening::Inst());
}

/* --- IgnoresStop --- */

void IgnoresStop::OnStopRequest(PhysicalLayerMonitor* apContext)
{
	LOGGER_BLOCK(apContext->mpLogger, LEV_INFO, "Ignoring Stop(): " << this->Name());
}

/* --- IgnoresStart --- */

void IgnoresStart::OnStartRequest(PhysicalLayerMonitor* apContext)
{
	LOGGER_BLOCK(apContext->mpLogger, LEV_INFO, "Ignoring Start(): " << this->Name());
}

void StopAndCloseRequestsCloseLayer::OnStopRequest(PhysicalLayerMonitor* apContext)
{
	apContext->mpPhys->AsyncClose();
	apContext->ChangeState(MonitorStateStopping::Inst());
}
	
void StopAndCloseRequestsCloseLayer::OnCloseRequest(PhysicalLayerMonitor* apContext)
{
	apContext->mpPhys->AsyncClose();
	apContext->ChangeState(MonitorStateClosing::Inst());
}

/* ----------------- Concrete ----------------- */


/* --- Stopped --- */

MonitorStateStopped MonitorStateStopped::mInstance;

/* --- Closed --- */

MonitorStateClosed MonitorStateClosed::mInstance;

void MonitorStateClosed::OnStartRequest(PhysicalLayerMonitor* apContext)
{
	if(apContext->ShouldBeTryingToOpen()) {
		apContext->mpPhys->AsyncOpen();
		apContext->ChangeState(MonitorStateOpening::Inst());
	}
}

void MonitorStateClosed::OnStopRequest(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateStopped::Inst());
}

/* --- Opening --- */

MonitorStateOpening MonitorStateOpening::mInstance;

void MonitorStateOpening::OnOpenFailure(PhysicalLayerMonitor* apContext)
{
	if(apContext->ShouldBeTryingToOpen()) {
		apContext->StartOpenTimer();
		apContext->ChangeState(MonitorStateWaiting::Inst());
	}
	else apContext->ChangeState(MonitorStateClosed::Inst());
}

void MonitorStateOpening::OnLayerOpen(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateOpen::Inst());	
}

/* --- Open --- */

MonitorStateOpen MonitorStateOpen::mInstance;

/* --- Waiting --- */

MonitorStateWaiting MonitorStateWaiting::mInstance;

void MonitorStateWaiting::OnCloseRequest(PhysicalLayerMonitor* apContext)
{	
	if(!apContext->ShouldBeTryingToOpen()) {
		apContext->CancelOpenTimer();
		apContext->ChangeState(MonitorStateClosed::Inst());
	}	
}

void MonitorStateWaiting::OnStopRequest(PhysicalLayerMonitor* apContext)
{
	apContext->CancelOpenTimer();
	apContext->ChangeState(MonitorStateStopped::Inst());
}

void MonitorStateWaiting::OnOpenTimeout(PhysicalLayerMonitor* apContext)
{
	apContext->mpPhys->AsyncOpen();
	apContext->ChangeState(MonitorStateOpening::Inst());
}

/* --- Closing --- */

MonitorStateClosing MonitorStateClosing::mInstance;

void MonitorStateClosing::OnStopRequest(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateStopping::Inst());
}

/* --- Stopping --- */

MonitorStateStopping MonitorStateStopping::mInstance;

void MonitorStateStopping::OnLayerClose(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateStopped::Inst());
}

}
