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
#include "PhysicalLayerMonitor.h"

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

	virtual void OnStartRequest(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnStopRequest(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnCloseRequest(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnOpenTimeout(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnOpenFailure(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnLayerOpen(PhysicalLayerMonitor* apContext) = 0;
	virtual void OnLayerClose(PhysicalLayerMonitor* apContext) = 0;

	virtual PhysicalLayerState GetState() const = 0;
	virtual std::string Name() const = 0;
};

class ExceptsOnLayerOpen : public virtual IMonitorState
{
public:
	void OnLayerOpen(PhysicalLayerMonitor* apContext);
};

class NotOpening : public ExceptsOnLayerOpen
{
public:	
	void OnOpenFailure(PhysicalLayerMonitor* apContext);
};

class NotOpen : public virtual IMonitorState
{
public:
	void OnLayerClose(PhysicalLayerMonitor* apContext);
};

class NotWaitingForTimer : public virtual IMonitorState
{
public:
	void OnOpenTimeout(PhysicalLayerMonitor* apContext);
};

class IgnoresClose : public virtual IMonitorState
{
public:
	void OnCloseRequest(PhysicalLayerMonitor* apContext);
};

class HandlesClose : public virtual IMonitorState
{
public:
	void OnLayerClose(PhysicalLayerMonitor* apContext);
};

class IgnoresStop : public virtual IMonitorState
{
public:
	void OnStopRequest(PhysicalLayerMonitor* apContext);
};

class IgnoresStart : public virtual IMonitorState
{
public:
	void OnStartRequest(PhysicalLayerMonitor* apContext);
};

template <class T, bool Retry>
class HandlesOpenFailure : public virtual IMonitorState
{
public:
	void OnOpenFailure(PhysicalLayerMonitor* apContext);
};



/* --- Concrete classes --- */

// disable "inherits via dominance warning", it's erroneous b/c base
// class is pure virtual and G++ correctly deduces this and doesn't care
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4250)
#endif

class MonitorStateStopped : public virtual IMonitorState,
	private NotOpening, private NotOpen, private NotWaitingForTimer, private IgnoresClose, private IgnoresStart, private IgnoresStop
{
	MACRO_MONITOR_SINGLETON(MonitorStateStopped, PLS_STOPPED);
};

class MonitorStateClosed : public virtual IMonitorState,
	private NotOpening, private NotOpen, private NotWaitingForTimer, private IgnoresClose
{
	MACRO_MONITOR_SINGLETON(MonitorStateClosed, PLS_CLOSED);

	void OnStartRequest(PhysicalLayerMonitor* apContext);
	void OnStopRequest(PhysicalLayerMonitor* apContext);
};

class MonitorStateOpening : public virtual IMonitorState,
	private NotOpen, private NotWaitingForTimer, private HandlesOpenFailure<MonitorStateClosed, true>, private IgnoresStart
{
	MACRO_MONITOR_SINGLETON(MonitorStateOpening, PLS_OPENING);

	void OnCloseRequest(PhysicalLayerMonitor* apContext);
	void OnStopRequest(PhysicalLayerMonitor* apContext);	
	void OnLayerOpen(PhysicalLayerMonitor* apContext);
};

class MonitorStateOpeningClosing : public virtual IMonitorState,
	private NotOpen, 
	private NotWaitingForTimer,
	private ExceptsOnLayerOpen,
	private HandlesOpenFailure<MonitorStateClosed, true>, 
	private IgnoresStart, 
	private IgnoresClose
{
	MACRO_MONITOR_SINGLETON(MonitorStateOpeningClosing, PLS_OPENING);
	
	void OnStopRequest(PhysicalLayerMonitor* apContext);		
};

class MonitorStateOpeningStopping : public virtual IMonitorState,
	private NotOpen, 
	private NotWaitingForTimer, 
	private ExceptsOnLayerOpen,
	private HandlesOpenFailure<MonitorStateStopped, false>, 
	private IgnoresStart, 
	private IgnoresClose,
	private IgnoresStop
{
	MACRO_MONITOR_SINGLETON(MonitorStateOpeningStopping, PLS_OPENING);	
};

class MonitorStateOpen : public virtual IMonitorState,
	private NotOpening, private NotWaitingForTimer, private IgnoresStart, private HandlesClose
{
	MACRO_MONITOR_SINGLETON(MonitorStateOpen, PLS_OPEN);

	void OnCloseRequest(PhysicalLayerMonitor* apContext);
	void OnStopRequest(PhysicalLayerMonitor* apContext);
};

class MonitorStateWaiting : public virtual IMonitorState,
	private NotOpening, private NotOpen, private IgnoresStart
{
	MACRO_MONITOR_SINGLETON(MonitorStateWaiting, PLS_WAITING);

	void OnCloseRequest(PhysicalLayerMonitor* apContext);
	void OnStopRequest(PhysicalLayerMonitor* apContext);
	void OnOpenTimeout(PhysicalLayerMonitor* apContext);
};

class MonitorStateClosing : public virtual IMonitorState,
	private NotOpening, private NotWaitingForTimer, private IgnoresStart, private IgnoresClose, private HandlesClose
{
	MACRO_MONITOR_SINGLETON(MonitorStateClosing, PLS_CLOSED);

	void OnStopRequest(PhysicalLayerMonitor* apContext);
};

class MonitorStateStopping : public virtual IMonitorState,
	private NotOpening, private NotWaitingForTimer, private IgnoresStart, private IgnoresClose, private IgnoresStop
{
	MACRO_MONITOR_SINGLETON(MonitorStateStopping, PLS_CLOSED);

	void OnLayerClose(PhysicalLayerMonitor* apContext);
};

template <class T, bool Retry>
void HandlesOpenFailure<T,Retry>::OnOpenFailure(PhysicalLayerMonitor* apContext)
{		
	if(Retry && apContext->ShouldBeTryingToOpen()) {
		apContext->StartOpenTimer();
		apContext->ChangeState(MonitorStateWaiting::Inst());			
	}
	else apContext->ChangeState(T::Inst());
}

#ifdef WIN32
#pragma warning(pop)
#endif

}

#endif

