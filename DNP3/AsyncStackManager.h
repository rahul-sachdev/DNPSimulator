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
#ifndef __ASYNC_STACK_MANAGER_H_
#define __ASYNC_STACK_MANAGER_H_


#include <map>
#include <vector>

#include <APL/Loggable.h>
#include <APL/TimerSourceASIO.h>
#include <APL/Threadable.h>
#include <APL/Thread.h>
#include <APL/DataInterfaces.h>
#include <APL/IPhysMonitor.h>
#include <APL/PhysicalLayerManager.h>
#include <APL/AsyncTaskScheduler.h>
#include <APL/Lock.h>
#include <APL/IOService.h>

#include "VtoDataInterface.h"

namespace apl {
	class IPhysicalLayerAsync;
	class Logger;
	class ICommandAcceptor;
	class IDataObserver;
}

namespace apl { namespace dnp {

class Port;
class Stack;

struct SlaveStackConfig;
struct MasterStackConfig;


/**
	The interface for C++ projects for dnp3. Provides an interface for
	starting/stopping master/slave protocol stacks. Any method may be
	called while the system is running.  Methods should only be called
	from a single thread at at a time.
*/
class AsyncStackManager : private Threadable, private Loggable
{
	public:
		/**
			@param apLogger - Logger to use for all other loggers
			@param aAutoRun	- Stacks begin execution immediately after being added
		*/
		AsyncStackManager(Logger* apLogger, bool aAutoRun = false);
		~AsyncStackManager();

		// All the io_service marshalling now occurs here. It's now safe to add/remove while the manager is running.

		/// Adds a TCPClient port, excepts if the port already exists
		void AddTCPClient(const std::string& arName, PhysLayerSettings, const std::string& arAddr, uint_16_t aPort);

		/// Adds a TCPServer port, excepts if the port already exists
		void AddTCPServer(const std::string& arName, PhysLayerSettings, const std::string& arEndpoint, uint_16_t aPort);

		/// Adds a Serial port, excepts if the port already exists
		void AddSerial(const std::string& arName, PhysLayerSettings, SerialSettings);

		/**
			Adds a master stack - Stack will automatically start running if Start() has been called or aAutoRun == true

			@param arPortName	Unique name of the port to with which to associate the stack. Will except if port doesn't exist
			@param arStackName	Unique name of the stack. Will except if the name already exists
			@param aLevel		Log filter level to use
			@param apPublisher	Interface to callback with measurements. The callback comes from an unknown network thread and should not be blocked.
			@param arCfg		Configuration data for the master stack
			@return				Thread-safe interface to use for dispatching commands to the master.
		*/
		ICommandAcceptor* AddMaster(const std::string& arPortName,
									const std::string& arStackName,
									FilterLevel aLevel,
									IDataObserver* apPublisher,
									const MasterStackConfig& arCfg);

		/**
			Adds a slave stack - Stack will automatically start running if Start() has been called or aAutoRun == true

			@param arPortName		Unique name of the port to with which to associate the stack. Will except if port doesn't exist
			@param arStackName		Unique name of the stack. Will except if the name already exists
			@param aLevel			Log filter level to use
			@param apCmdAcceptor	Interface to callback with measurements. The callback comes from an unknown network thread and should not be blocked.
			@param arCfg			Configuration data for the slave stack
			@return					Thread-safe interface to use for writing new measurement values to the slave.
		*/
		IDataObserver* AddSlave(const std::string& arPortName,
								const std::string& arStackName,
								FilterLevel aLevel,
								ICommandAcceptor* apCmdAcceptor,
								const SlaveStackConfig&);

		/**
			Adds a VTO channel to a prexisting stack (master or slave).
			This function should be used for advanced control of a VTO channel,
			where the implementer wants to control the end VTO stream as a byte
			array.  Otherwise, the implementer should look at
			AsyncStackManager::StartVtoRouter() as a simpler way of connecting a
			port (such as a local TCP service) to the VTO stream.

			@param arPortName			Unique name of the port to which the
										router should associate.
			@param arStackName			Unique name of the stack.
			@param aVtoChannelId		Unique channel ID for the VTO circuit.
			@param apOnDataCallback		Interface to callback with received
										data.  The callback comes from an
										unknown network thread, and should not
										be blocked.
			@param aReservedOctetCount	The minimum number of octets to reserve
										in the DNP3 application layer for VTO
										data related to this virtual channel.

			@return						Interface to use for writing
										new VTO data from the master to the
										slave.

			@throw ArgumentException	if arPortName/arStackName doesn't exist
										or if the VTO channel ID is already
										bound for that stack
		 */
		IVtoWriter* AddVtoChannel(const std::string& arPortName,
						const std::string& arStackName,
						byte_t aVtoChannelId,
						IVtoCallbacks* apOnDataCallback,
						size_t aReservedOctetCount = 0);

		/**
			Removes an existing VTO channel that was created using
			AsyncStackManager::AddVtoChannel(), stopping callbacks.

			@param apOnDataCallback		Callback interface previously registered
										in AddVtoChannel()

			@throw ArgumentException if apOnDataCallback doesn't exist
		*/
		void RemoveVtoChannel(IVtoCallbacks* apOnDataCallback);

		/**
			Starts the VtoRouter for the specified port and stack.
			A VtoRouter acts as a conduit, where the VTO stream is funneled
			between the arStackName (which also defines a master/slave port) and
			the arPortName (such as a TCP/IP daemon).  If the implementer wants
			to terminate the VTO stream in the application itself, he/she should
			look at the AsyncStackManager::AddVtoChannel() routine.

			@param arPortName			Unique name of the port to which the
										router should associate.
			@param arStackName			Unique name of the stack.
			@param aVtoChannelId		Unique channel ID for the VTO circuit.
			@param apOnDataCallback		Interface to callback with received
										data.  The callback comes from an
										unknown network thread, and should not
										be blocked.
			@param aReservedOctetCount	The minimum number of octets to reserve
										in the DNP3 application layer for VTO
										data related to this virtual channel.

			@return						Interface to use for writing
										new VTO data from the master to the
										slave.

			@throw ArgumentException	if arPortName/arStackName doesn't exist
										or if the VTO channel ID is already
										bound for that stack
		 */
		void StartVtoRouter(const std::string& arPortName,
						const std::string& arStackName,
						byte_t aVtoChannelId,
						size_t aReservedOctetCount = 0);

		/**
			Shutdown a VtoRouter on the specified port, stack, and VTO channel.

			@param arPortName			Unique name of the port.
			@param arStackName			Unique name of the stack.

			@throw ArgumentException	if arPortName/arStackName doesn't exist
		*/
		void StopVtoRouter(const std::string& arPortName,
						const std::string& arStackName,
						byte_t aVtoChannelId);

		/**
			Shutdown all VtoRouter instances on the specified port and stack.

			@param arPortName			Unique name of the port.
			@param arStackName			Unique name of the stack.
			@param aVtoChannelId		Unique channel ID for the VTO circuit.

			@throw ArgumentException	if arPortName/arStackName doesn't exist
										or if the VTO channel ID is already
										bound for that stack
		*/
		void StopVtoRouter(const std::string& arPortName,
						const std::string& arStackName);

		/// Remove a port and all associated stacks
		void RemovePort(const std::string& arPortName);

		/// Remove only a single stack
		void RemoveStack(const std::string& arStackName);

		/// @return a vector of all the stack names
		std::vector<std::string> GetStackNames();

		/// @return a vector of all the port names
		std::vector<std::string> GetPortNames();

		/// Start the thead if it isn't running
		void Start();

		/// Stop the thread, doesn't delete anything until the stack manager destructs
		void Stop();

	private:

		Port* AllocatePort(const std::string& arName);
		Port* CreatePort(const std::string& arName, IPhysicalLayerAsync* apPhys, Logger* apLogger, millis_t aOpenDelay, IPhysMonitor* apObserver);
		Port* GetPort(const std::string& arName);
		Port* GetPortByStackName(const std::string& arStackName);
		Port* GetPortPointer(const std::string& arName);

		void Run();

		/// Remove a stack
		void SeverStack(Port* apPort, const std::string& arStackName);

		void OnAddStack(const std::string& arStackName, Stack* apStack, Port* apPort, uint_16_t aAddress);
		void CheckForJoin();

		bool mRunASIO;
		bool mRunning;
		size_t NumStacks() { return mStackToPort.size(); }

		std::vector<std::string> StacksOnPort(const std::string& arPortName);

	protected:
		IOService mService;
		TimerSourceASIO mTimerSrc;

	private:
		PhysicalLayerManager mMgr;
		AsyncTaskScheduler mScheduler;
		Thread mThread;

		typedef std::map<std::string, Port*> PortMap;
		PortMap mStackToPort;		/// maps a stack name a port instance
		PortMap mPortToPort;		/// maps a port name to a port instance

		typedef std::map<std::string, size_t> mPortCount;	/// how many stacks per port
};

}}

#endif
