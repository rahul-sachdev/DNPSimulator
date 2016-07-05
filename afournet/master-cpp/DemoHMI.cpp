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

#include "MasterDemo.h"

#include <signal.h>

#include <opendnp3/APL/Log.h>
#include <opendnp3/APL/LogToFile.h>
#include <opendnp3/APL/Lock.h>

using namespace std;
using namespace apl;
using namespace apl::dnp;

SigLock gLock;
MasterDemoBase* gpDemo = NULL;

void SetDemo(MasterDemoBase* apDemo)
{
	CriticalSection cs(&gLock);
	gpDemo = apDemo;
}

void Terminate(int sig)
{
	CriticalSection cs(&gLock);
	std::cout << "Signal " << sig << ", shutdown... " << std::endl;
	if (gpDemo)
		gpDemo->Shutdown();
}

/*
 * Command line syntax:
 *
 *    ./demo-master-cpp [local-dnp3] [remote-dnp3] [remote-ip] [remote-port]
 *
 * Defaults:
 *
 *    local-dnp3     100
 *    remote-dnp3    1
 *    remote-ip      127.0.0.1
 *    remote-port    4999
 */
int main(int argc, char* argv[])
{
	// Default values
	unsigned local_dnp3  = 100;
	unsigned remote_dnp3 = 1;
	string   remote_ip   = "127.0.0.1";
	unsigned remote_port = 5000;
	// Parse the command line arguments using a "fall-through"
	// switch statement.
	if (argc > 1 && strcmp("help", argv[1]) == 0) {
		cout << argv[0] << " [local-dnp3] [remote-dnp3] [remote-ip] [remote-port]" << endl;
		return -1;
	}
	
	switch (argc) {
	case 5:
		{
			istringstream iss(argv[4]);
			iss >> remote_port;
		}
	case 4:
		remote_ip = argv[3];
	case 3:
		{
			istringstream iss(argv[2]);
			iss >> remote_dnp3;
		}
	case 2:
		{
			istringstream iss(argv[1]);
			iss >> local_dnp3;
		}
	}

	// Create a log object for the stack to use and configure it
	// with a subscriber that print alls messages to the stdout.
	EventLog log;
	log.AddLogSubscriber(LogToStdio::Inst());
	// Specify a FilterLevel for the stack/physical layer to use.
	// Log statements with a lower priority will not be logged.
	const FilterLevel LOG_LEVEL = LEV_EVENT;

	// Create our demo application that handles commands and
	// demonstrates how to publish data give it a logger with a
	// unique name and log level.
	MasterDemoApp app(log.GetLogger(LOG_LEVEL, "East Wing "));

	// This is the main point of interaction with the stack. The
	// AsyncStackManager object instantiates master/slave DNP
	// stacks, as well as their physical layers.
	AsyncStackManager mgr(log.GetLogger(LOG_LEVEL, "dnp"));

	// Connect via a TCPv4Client socket to a slave.  The server will
	// wait 3000 ms in between failed bind calls.
	mgr.AddTCPv4Client(
		"tcpclient",
		PhysLayerSettings(LOG_LEVEL, 3000),
		TcpSettings(remote_ip.c_str(), remote_port)
	);

	// The master config object for a master. The default are
	// useable, but understanding the options are important.
	MasterStackConfig stackConfig;

	// Override the default link addressing
	stackConfig.link.LocalAddr  = local_dnp3;
	stackConfig.link.RemoteAddr = remote_dnp3;

	// Set the app instance as a callback for state change notices
	stackConfig.master.mpObserver = &app;

	// Create a new master on a previously declared port, with a
	// name, log level, command acceptor, and config info This
	// returns a thread-safe interface used for processing Master
	// messages.
	app.SetCommandAcceptor(
		mgr.AddMaster(
			"tcpclient",           // port name
			"master",              // stack name
			LOG_LEVEL,             // log filter level
			app.GetDataObserver(), // callback for data processing
			stackConfig            // stack configuration
		)
	);

	// Configure signal handlers so we can exit gracefully
	SetDemo(&app);
	signal(SIGTERM, &Terminate);
	signal(SIGABRT, &Terminate);
	signal(SIGINT,  &Terminate);

	app.Run();

	SetDemo(NULL);

	return 0;
}

