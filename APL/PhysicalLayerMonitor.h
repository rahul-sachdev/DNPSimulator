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
#ifndef __PHYSICAL_LAYER_MONITOR_H_
#define __PHYSICAL_LAYER_MONITOR_H_


#include "IHandlerAsync.h"
#include "TimerInterfaces.h"
#include "IPhysicalLayerObserver.h"

#include <APL/Lock.h>

#include <set>

namespace apl
{

class IPhysicalLayerAsync;
class IMonitorState;
class IPhysicalLayerObserver;

/** Manages the lifecycle of a physical layer
  */
class PhysicalLayerMonitor : public IHandlerAsync
{
	friend class IgnoresClose;
	friend class IgnoresStart;
	friend class IgnoresStop;
	friend class HandlesClose;
	friend class MonitorStateClosed;
	friend class MonitorStateStopped;
	friend class MonitorStateOpening;
	friend class MonitorStateOpen;
	friend class MonitorStateClosing;
	friend class MonitorStateStopping;
	friend class MonitorStateWaiting;
	friend class StopAndCloseRequestsCloseLayer;


public:
	PhysicalLayerMonitor(Logger*, IPhysicalLayerAsync*, ITimerSource*, millis_t aOpenRetry);
	~PhysicalLayerMonitor();

	/** Begin monitor execution - Idempotent*/
	void Start();

	/** Close the physical layer if it's open, may re-open - Idempotent*/
	void Close();

	/** Permanently stop the monitor, further calls to Start() will do nothing - Idempotent */
	void Stop();

	PhysicalLayerState GetState();

	/** Add an observer to the set of state callbacks */
	void AddObserver(IPhysicalLayerObserver* apObserver);

	/** Blocks until the monitor has completely and permanently stopped */
	void WaitForStopped();

protected:

	
	virtual void OnPhysicalLayerOpenCallback() = 0;
	virtual void OnPhysicalLayerCloseCallback() = 0;

	/*
	virtual void OnPhysicalLayerOpenFailure() = 0;
	*/
		
	/** Default behavior is always trying to connect, but this is overridable */
	virtual bool ShouldBeTryingToOpen() { return true; }

	/// Begins the open timer
	void StartOpenTimer();

	IPhysicalLayerAsync* mpPhys;

private:
	
	ITimerSource* mpTimerSrc;
	ITimer* mpOpenTimer;
	IMonitorState* mpState;

	/* --- Actions for the states to call --- */

	/// Internal function used to change the state
	void ChangeState(IMonitorState* apState);

	/// Internal callback when open timer expires
	void OnOpenTimerExpiration();

	/// Cancels the open timer
	void CancelOpenTimer();

	/* --- Internal helper functions --- */

	SigLock mLock;
	const millis_t M_OPEN_RETRY;

	// Implement from IHandlerAsync - Try to reconnect using a timer
	void _OnOpenFailure();
	void _OnLowerLayerUp();
	void _OnLowerLayerDown();

	typedef std::set<IPhysicalLayerObserver*> ObserverSet;
	ObserverSet mObservers;

};
}

#endif
