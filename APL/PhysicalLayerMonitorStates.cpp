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
#include "IPhysicalLayerAsync.h"

namespace apl
{

/* --- ExceptsOnLayerOpen --- */

void ExceptsOnLayerOpen::OnLayerOpen(PhysicalLayerMonitor* apContext)
{
	throw InvalidStateException(LOCATION, "Not opening: " + this->Name());
}

/* --- NotOpening --- */

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
	if(apContext->ShouldBeTryingToOpen()) {
		apContext->mpPhys->AsyncOpen();
		apContext->ChangeState(MonitorStateOpening::Inst());
	}
	else apContext->ChangeState(MonitorStateClosed::Inst());
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

void MonitorStateOpening::OnStopRequest(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateOpeningStopping::Inst());
	apContext->mpPhys->AsyncClose();
	
}
	
void MonitorStateOpening::OnCloseRequest(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateOpeningClosing::Inst());
	apContext->mpPhys->AsyncClose();	
}

void MonitorStateOpening::OnLayerOpen(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateOpen::Inst());	
}

/* --- OpeningClosing --- */

MonitorStateOpeningClosing MonitorStateOpeningClosing::mInstance;

void MonitorStateOpeningClosing::OnStopRequest(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateOpeningStopping::Inst());
}

/* --- OpeningStopping --- */

MonitorStateOpeningStopping MonitorStateOpeningStopping::mInstance;

/* --- Open --- */

MonitorStateOpen MonitorStateOpen::mInstance;

void MonitorStateOpen::OnCloseRequest(PhysicalLayerMonitor* apContext)
{	
	apContext->ChangeState(MonitorStateClosing::Inst());
	apContext->mpPhys->AsyncClose();
}

void MonitorStateOpen::OnStopRequest(PhysicalLayerMonitor* apContext)
{	
	apContext->ChangeState(MonitorStateStopping::Inst());
	apContext->mpPhys->AsyncClose();
}

/* --- Waiting --- */

MonitorStateWaiting MonitorStateWaiting::mInstance;

void MonitorStateWaiting::OnCloseRequest(PhysicalLayerMonitor* apContext)
{	
	if(!apContext->ShouldBeTryingToOpen()) {
		apContext->ChangeState(MonitorStateClosed::Inst());
		apContext->CancelOpenTimer();		
	}	
}

void MonitorStateWaiting::OnStopRequest(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateStopped::Inst());
	apContext->CancelOpenTimer();	
}

void MonitorStateWaiting::OnOpenTimeout(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateOpening::Inst());
	apContext->mpPhys->AsyncOpen();	
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
