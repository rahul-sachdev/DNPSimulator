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
#ifndef __ASYNC_PHYS_LAYER_MONITOR_H_
#define __ASYNC_PHYS_LAYER_MONITOR_H_


#include "IHandlerAsync.h"
#include "TimerInterfaces.h"
#include "Logger.h"
#include "IPhysicalLayerObserver.h"

#include <set>

namespace apl
{

class IPhysicalLayerAsync;

/** Class keeps a physical layer open by kicking off
  *	AsyncOpen requests on a timer
  */
class AsyncPhysLayerMonitor : public IHandlerAsync
{
public:
	AsyncPhysLayerMonitor(Logger*, IPhysicalLayerAsync*, ITimerSource*, millis_t aOpenRetry);
	~AsyncPhysLayerMonitor();

	void Start();
	void Stop();

	/// will close if we are open, since we will auto reconnect
	void Reconnect();

	PhysicalLayerState GetState() {
		return mState;
	}

	void AddMonitor(IPhysicalLayerObserver* apMonitor);

	void WaitForStopped();

	bool IsStopping();

protected:

	IPhysicalLayerAsync* mpPhys;
	ITimerSource* mpTimerSrc;
	ITimer* mpOpenTimer;
	LogVariable mPortState;


	virtual void OnPhysicalLayerOpen() = 0;
	virtual void OnPhysicalLayerClose() = 0;
	virtual void OnPhysicalLayerOpenFailure() {}
	virtual void OnStateChange(PhysicalLayerState) {}

private:

	/// Internal callback when open timer expires
	void OnOpenTimerExpiration();

	/// Internal function used to change the state
	void ChangeState(PhysicalLayerState);

	/// Blocking function that waits for the port to stop (physical layer to be closed)
	void WaitForState(PhysicalLayerState aState);

	SigLock mLock;
	PhysicalLayerState mState;
	bool mIsStopping;
	const millis_t M_OPEN_RETRY;

	// Implement from IHandlerAsync - Try to reconnect using a timer
	void _OnOpenFailure();
	void _OnLowerLayerUp();
	void _OnLowerLayerDown();

	typedef std::set<IPhysicalLayerObserver*> MonitorSet;
	MonitorSet mMonitors;

};
}

#endif
