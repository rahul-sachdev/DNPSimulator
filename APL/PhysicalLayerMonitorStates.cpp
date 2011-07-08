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

#include "PhysicalLayerMonitor.h"
#include "Exception.h"

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

/* --- Stopped --- */

MonitorStateStopped MonitorStateStopped::mInstance;

/* --- Closed --- */

MonitorStateClosed MonitorStateClosed::mInstance;

void MonitorStateClosed::OnStop(PhysicalLayerMonitor* apContext) 
{ 
	apContext->ChangeState(MonitorStateStopped::Inst());
}

}
