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
#ifndef __I_PHYS_MONITOR_H_
#define __I_PHYS_MONITOR_H_

#include <string>

namespace apl
{

enum PhysLayerState {
    PLS_CLOSED,		// layer is offline and idle
    PLS_OPENING,	// layer is trying to open
    PLS_WAITING,
    PLS_OPEN,		// layer is open
    PLS_STOPPED		// stopped and will no longer dispatch events
};

std::string ConvertPhysLayerStateToString(PhysLayerState aState);

class IPhysMonitor
{
public:

	virtual ~IPhysMonitor() {}

	virtual void OnStateChange(PhysLayerState) = 0;

};

}

#endif
