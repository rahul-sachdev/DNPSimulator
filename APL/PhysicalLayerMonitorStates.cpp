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

void IgnoresClose::OnClose(PhysicalLayerMonitor* apContext)
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

void IgnoresStop::OnStop(PhysicalLayerMonitor* apContext)
{
	LOGGER_BLOCK(apContext->mpLogger, LEV_INFO, "Ignoring Stop(): " << this->Name());
}

/* --- IgnoresStart --- */

void IgnoresStart::OnStart(PhysicalLayerMonitor* apContext)
{
	LOGGER_BLOCK(apContext->mpLogger, LEV_INFO, "Ignoring Start(): " << this->Name());
}

/* ----------------- Concrete ----------------- */


/* --- Stopped --- */

MonitorStateStopped MonitorStateStopped::mInstance;

/* --- Closed --- */

MonitorStateClosed MonitorStateClosed::mInstance;

void MonitorStateClosed::OnStart(PhysicalLayerMonitor* apContext)
{
	apContext->mpPhys->AsyncOpen();
	apContext->ChangeState(MonitorStateOpening::Inst());
}

void MonitorStateClosed::OnStop(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateStopped::Inst());
}

/* --- Opening --- */

MonitorStateOpening MonitorStateOpening::mInstance;

void MonitorStateOpening::OnStop(PhysicalLayerMonitor* apContext)
{
	apContext->mpPhys->AsyncClose();
	apContext->ChangeState(MonitorStateStopping::Inst());
}

void MonitorStateOpening::OnClose(PhysicalLayerMonitor* apContext)
{
	apContext->mpPhys->AsyncClose();
	apContext->ChangeState(MonitorStateClosing::Inst());
}

void MonitorStateOpening::OnOpenFailure(PhysicalLayerMonitor* apContext)
{
	apContext->StartOpenTimer();
}

void MonitorStateOpening::OnLayerOpen(PhysicalLayerMonitor* apContext)
{
	apContext->ChangeState(MonitorStateOpen::Inst());
}

/* --- Open --- */

MonitorStateOpen MonitorStateOpen::mInstance;

void MonitorStateOpen::OnStop(PhysicalLayerMonitor* apContext)
{
	apContext->mpPhys->AsyncClose();
	apContext->ChangeState(MonitorStateStopping::Inst());
}

void MonitorStateOpen::OnClose(PhysicalLayerMonitor* apContext)
{
	apContext->mpPhys->AsyncClose();
	apContext->ChangeState(MonitorStateClosing::Inst());
}

/* --- Waiting --- */

void MonitorStateWaiting::OnStop(PhysicalLayerMonitor* apContext)
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

void MonitorStateClosing::OnStop(PhysicalLayerMonitor* apContext)
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
