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
#ifndef __PORT_H_
#define __PORT_H_


#include "LinkLayerRouter.h"

#include <APL/Loggable.h>
#include <APL/Lock.h>

#include <vector>

namespace apl
{

class Logger;
class IPhysicalLayerAsync;
class ITimerSource;
class AsyncTaskGroup;

namespace dnp
{

class Stack;
class AsyncStackManager;

template <class T, class U>
static std::vector<U> GetKeys(T& arMap)
{
	std::vector<U> ret;
	for(typename T::iterator i = arMap.begin(); i != arMap.end(); ++i) {
		ret.push_back(i->first);
	}
	return ret;
}

class Port : public Loggable, public IPhysMonitor
{
	struct StackRecord {
		StackRecord() : pStack(NULL), route()
		{}

		StackRecord(Stack* apStack, const LinkRoute& arRoute) :
			pStack(apStack), route(arRoute)
		{}

		Stack* pStack;
		LinkRoute route;
	};

public:

	Port(const std::string& arName, Logger*, ITimerSource* apTimerSrc, IPhysicalLayerAsync*, millis_t aOpenDelay);

	AsyncTaskGroup* GetGroup() {
		return mpGroup;
	}
	void BindStackToPort(const std::string& arStackName, Stack* apStack, const LinkRoute& arRoute);
	void RemoveStackFromPort(const std::string& arStackName);

	std::string Name() {
		return mName;
	}

//	void AddObserver(

	/// Events from the router
	void OnStateChange(PhysLayerState);

	/// Blocking function that waits for the port to stop (physical layer to be closed)
	void WaitForStop();

private:

	SigLock mLock;
	std::string mName;
	LinkLayerRouter mRouter;
	AsyncTaskGroup* mpGroup;
	IPhysicalLayerAsync* mpPhys;
	PhysLayerState mState;

private:

	typedef std::map<std::string, StackRecord> StackMap;
	StackMap mStackMap;

};

}
}


#endif

