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

#ifndef __PHYS_LAYER_MONITOR_STATES_H_
#define __PHYS_LAYER_MONITOR_STATES_H_

#include "IPhysicalLayerObserver.h"
#include "Singleton.h"

#define MACRO_MONITOR_SINGLETON(type, state) \
	MACRO_NAME_SINGLETON_INSTANCE(type) \
	PhysicalLayerState GetState() const { return state; }

namespace apl
{

class PhysicalLayerMonitor;

/* --- Base classes --- */

class IMonitorState
{
public:

	virtual void OnStart(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnStop(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnClose(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnOpenTimeout(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnOpenFailure(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnLayerOpen(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnLayerClose(PhysicalLayerMonitor* apContext) = 0;

	virtual PhysicalLayerState GetState() const = 0;
	virtual std::string Name() const = 0;
};

class NotOpening : public virtual IMonitorState
{
public:	
	virtual void OnLayerOpen(PhysicalLayerMonitor* apContext);
	virtual void OnOpenFailure(PhysicalLayerMonitor* apContext);
};

class NotOpen : public virtual IMonitorState
{
public:	
	virtual void OnLayerClose(PhysicalLayerMonitor* apContext);
};

/* --- Concrete classes --- */

class MonitorStateStopped : public NotOpening, public NotOpen
{
	MACRO_MONITOR_SINGLETON(MonitorStateStopped, PLS_STOPPED);

	void OnStart(PhysicalLayerMonitor* apContext) {} // ignore
	void OnStop(PhysicalLayerMonitor* apContext) {} // ignore
	void OnClose(PhysicalLayerMonitor* apContext) {} // ignore	
	void OnOpenTimeout(PhysicalLayerMonitor* apContext) {}	
};


class MonitorStateClosed : public NotOpening, public NotOpen
{
	MACRO_MONITOR_SINGLETON(MonitorStateClosed, PLS_CLOSED);

	void OnStart(PhysicalLayerMonitor* apContext) {}
	void OnStop(PhysicalLayerMonitor* apContext); 
	void OnClose(PhysicalLayerMonitor* apContext) {}
	void OnOpenTimeout(PhysicalLayerMonitor* apContext) {} 		
};

}

#endif

