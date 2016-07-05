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

#ifndef __SLAVE_DEMO_H_
#define __SLAVE_DEMO_H_

#include <opendnp3/APL/Log.h>
#include <opendnp3/APL/IOServiceThread.h>
#include <opendnp3/APL/CommandQueue.h>
#include <opendnp3/APL/FlexibleDataObserver.h>
#include <opendnp3/APL/IPhysicalLayerAsync.h>
#include <opendnp3/APL/IOService.h>
#include <opendnp3/APL/TimerSourceASIO.h>
#include <opendnp3/APL/PostingNotifierSource.h>
#include <opendnp3/APL/LogToStdio.h>
#include <opendnp3/APL/Loggable.h>

#include <opendnp3/DNP3/SlaveStackConfig.h>
#include <opendnp3/DNP3/AsyncStackManager.h>

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
namespace apl

{
namespace dnp
{

/**
	This class takes care of all the plumbing and bingings between application code and the stack.
	Inherited clases on have to implement the ICommandHandler interface.
*/
class SlaveDemoBase : protected ICommandHandler, private IOService, public IOServiceThread
{
public:
	SlaveDemoBase(Logger* apLogger);

	// returns an interface the slave stack can use to notify when a command arrives
	ICommandAcceptor* GetCmdAcceptor() {
		return &mCommandQueue;
	}

	// Tell the io_service to exit
	void Shutdown();
	timeval prevRunTime;
	timeval currRunTime;
private:

	/** OnCommandNotify has been marshalled to the application thread. It
		causes a single command to be processed */
	void OnCommandNotify();

	// The CommandQueue object serves as a thread-safe cache and notification mechanism. It implements the
	// ICommandAcceptor/IResponseAcceptor/ICommandSource interface, which is used by the slave/application
	// during control sequences
	CommandQueue mCommandQueue;
	PostingNotifierSource mPostSource;	// bridge between Notifiers and IO services that marshalls method calls between threads
	TimerSourceASIO mTimerSource;		// boost timer source, interface into the io service system
	ITimer* mpInfiniteTimer;			// timer used to keep the boost io service alive
	ICommandSource* mpCommandSource;	// The source for getting and executing commands.
};


/**
	This class demonstrates application logic, using APL scada types to respond to controls
	and updating data. In addition to the ICommandSource/ICommandHandler system, the
	IDataObserver interface gives transactional write capability to the slave's database.
*/
class SlaveDemoApp : public SlaveDemoBase
{
public:
	SlaveDemoApp(Logger* apLogger);

	void SetDataObserver(IDataObserver* apObserver);

	/**
		Processes a setpoint (Analog output) request from the master. This is where the user
		code will do some work. In this example, we'll just update the slave database.
		The return value is what the slave returns to the remote master.
	 */
	CommandStatus HandleControl(Setpoint& aControl, size_t aIndex);

	/**
		The same, but for binary outputs
	 */
	CommandStatus HandleControl(BinaryOutput& aControl, size_t aIndex);

private:
	int mCountSetPoints;    // count how many setpoints we recieve to demonstrate counters
	int mCountBinaryOutput; // count how many binary controls we recieve to demonstrate counters

	IDataObserver* mpObserver;  // The data sink for updating the slave database.
};

}
}

#endif
