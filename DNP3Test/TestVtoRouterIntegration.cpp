/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership.  Green Enery
 * Corp licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <boost/test/unit_test.hpp>
#include <APLTestTools/TestHelpers.h>
#include <APLTestTools/MockCommandAcceptor.h>

#include <APL/Log.h>
#include <APL/LogToStdio.h>
#include <APL/LogToFile.h>
#include <APL/FlexibleDataObserver.h>

#include <DNP3/AsyncStackManager.h>
#include <DNP3/SlaveStackConfig.h>
#include <DNP3/MasterStackConfig.h>
#include <DNP3/VtoRouterSettings.h>

using namespace apl;
using namespace apl::dnp;

/** Platforms have different reserved port ranges */
 
#if defined(WIN32)
	/* Windows platform */
	#define PORT_VALUE			(50000)
#else
	/* Generic Linux platform */
	#define PORT_VALUE			(30000)	
#endif

BOOST_AUTO_TEST_SUITE(VtoRouterIntegrationSuite)
/*
BOOST_AUTO_TEST_CASE(MasterToSlave)
{
	const boost::uint16_t port = PORT_VALUE;
	const FilterLevel level = LEV_INFO;

	EventLog log;
	log.AddLogSubscriber(LogToStdio::Inst());
	LogToFile ltf(&log, "integration.log");
	FlexibleDataObserver fdo;
	FlexibleDataObserver tunneledData;
	MockCommandAcceptor cmdAcceptor;
	AsyncStackManager mgr(log.GetLogger(level, "test"));
	
	mgr.AddTCPClient("client", PhysLayerSettings(), "127.0.0.1", port);
	mgr.AddTCPServer("server", PhysLayerSettings(), "127.0.0.1", port);
	
	SlaveStackConfig slave = SlaveStackConfig();
	slave.device = DeviceTemplate(10,10,10,10,10,10,10);

	mgr.AddSlave("client", "slave", level, &cmdAcceptor, slave);
	mgr.AddMaster("server", "master", level, &fdo, MasterStackConfig());
	
	mgr.AddTCPClient("vtoserver1", PhysLayerSettings(), "127.0.0.1", port+10);
	mgr.AddTCPServer("vtoserver2", PhysLayerSettings(), "127.0.0.1", port+20);
	
	mgr.StartVtoRouter("vtoserver1", "master", VtoRouterSettings(0, false));
	mgr.StartVtoRouter("vtoserver2", "slave", VtoRouterSettings(0, true));

	mgr.AddTCPClient("tunneled_client", PhysLayerSettings(), "127.0.0.1", port+20);
	mgr.AddTCPServer("tunneled_server", PhysLayerSettings(), "127.0.0.1", port+10);

	mgr.AddSlave("tunneled_server", "tunneled_slave", level, &cmdAcceptor, slave);
	mgr.AddMaster("tunneled_client", "tunneled_master", level, &tunneledData, MasterStackConfig());
	
	BOOST_REQUIRE_EQUAL(tunneledData.GetTotalCount(),0);

	mgr.Start();

	Thread::SleepFor(10000);

	BOOST_REQUIRE_GT(fdo.GetTotalCount(),0);

	std::cout << "Tunneled: " << tunneledData.GetTotalCount() << std::endl;
	BOOST_REQUIRE_GT(tunneledData.GetTotalCount(),0);
	

	mgr.Stop();
}

BOOST_AUTO_TEST_CASE(SlaveToMaster)
{
	const boost::uint16_t port = PORT_VALUE;
	const FilterLevel level = LEV_INFO;

	EventLog log;
	log.AddLogSubscriber(LogToStdio::Inst());
	LogToFile ltf(&log, "integration.log");
	FlexibleDataObserver fdo;
	FlexibleDataObserver tunneledData;
	MockCommandAcceptor cmdAcceptor;
	AsyncStackManager mgr(log.GetLogger(level, "test"));
	
	mgr.AddTCPClient("client", PhysLayerSettings(), "127.0.0.1", port);
	mgr.AddTCPServer("server", PhysLayerSettings(), "127.0.0.1", port);
	
	SlaveStackConfig slave = SlaveStackConfig();
	slave.device = DeviceTemplate(10,10,10,10,10,10,10);

	mgr.AddSlave("client", "slave", level, &cmdAcceptor, slave);
	mgr.AddMaster("server", "master", level, &fdo, MasterStackConfig());
	
	mgr.AddTCPClient("vtoserver1", PhysLayerSettings(), "127.0.0.1", port+10);
	mgr.AddTCPServer("vtoserver2", PhysLayerSettings(), "127.0.0.1", port+20);
	
	mgr.StartVtoRouter("vtoserver1", "slave", VtoRouterSettings(0, false));
	mgr.StartVtoRouter("vtoserver2", "master", VtoRouterSettings(0, true));

	mgr.AddTCPClient("tunneled_client", PhysLayerSettings(), "127.0.0.1", port+20);
	mgr.AddTCPServer("tunneled_server", PhysLayerSettings(), "127.0.0.1", port+10);

	mgr.AddSlave("tunneled_server", "tunneled_slave", level, &cmdAcceptor, slave);
	mgr.AddMaster("tunneled_client", "tunneled_master", level, &tunneledData, MasterStackConfig());
	
	BOOST_REQUIRE_EQUAL(tunneledData.GetTotalCount(),0);

	mgr.Start();

	Thread::SleepFor(10000);

	BOOST_REQUIRE_GT(fdo.GetTotalCount(),0);

	std::cout << "Tunneled: " << tunneledData.GetTotalCount() << std::endl;
	BOOST_REQUIRE_GT(tunneledData.GetTotalCount(),0);

	mgr.Stop();
}
*/

BOOST_AUTO_TEST_CASE(MultiThreadedSlaveToMaster)
{
	const boost::uint16_t port = PORT_VALUE;
	const FilterLevel level = LEV_INFO;

	EventLog log;
	EventLog localLog;
	EventLog remoteLog;
	EventLog tunneledMaster;
	EventLog tunneledSlave;
	localLog.AddLogSubscriber(LogToStdio::Inst());
	remoteLog.AddLogSubscriber(LogToStdio::Inst());
	LogToFile ltf(&localLog, "local.log");
	LogToFile ltf2(&remoteLog, "remote.log");
	LogToFile ltf3(&tunneledMaster, "tmaster.log");
	LogToFile ltf4(&tunneledSlave, "tslave.log");

	FlexibleDataObserver fdo;
	FlexibleDataObserver tunneledData;
	MockCommandAcceptor cmdAcceptor;
	AsyncStackManager local(localLog.GetLogger(level, "local"));
	AsyncStackManager remote(remoteLog.GetLogger(level, "remote"));
	
	local.AddTCPClient("client", PhysLayerSettings(), "127.0.0.1", port);
	remote.AddTCPServer("server", PhysLayerSettings(), "127.0.0.1", port);
	
	local.AddMaster("client", "master", level, &fdo, MasterStackConfig());
	remote.AddSlave("server", "slave", level, &cmdAcceptor, SlaveStackConfig());
	
	local.AddTCPServer("vto_server", PhysLayerSettings(), "127.0.0.1", port+10);
	remote.AddTCPClient("vto_client", PhysLayerSettings(), "127.0.0.1", port+20);
	
	local.StartVtoRouter("vto_server", "master", VtoRouterSettings(0, true));
	remote.StartVtoRouter("vto_client", "slave", VtoRouterSettings(0, false));

	AsyncStackManager mgrTunneledMaster(tunneledMaster.GetLogger(level, "tmaster"));
	AsyncStackManager mgrTunneledSlave(tunneledSlave.GetLogger(level, "tslave"));

	mgrTunneledMaster.AddTCPClient("tunneled_client", PhysLayerSettings(), "127.0.0.1", port+10);
	mgrTunneledSlave.AddTCPServer("tunneled_server", PhysLayerSettings(), "127.0.0.1", port+20);

	mgrTunneledSlave.AddSlave("tunneled_server", "tunneled_slave", level, &cmdAcceptor, SlaveStackConfig());
	mgrTunneledMaster.AddMaster("tunneled_client", "tunneled_master", level, &tunneledData, MasterStackConfig());
	
	local.Start();
	remote.Start();

	Thread::SleepFor(3000);

	BOOST_REQUIRE_EQUAL(tunneledData.GetTotalCount(),0);

	mgrTunneledMaster.Start();
	mgrTunneledSlave.Start();

	Thread::SleepFor(5000);

	BOOST_REQUIRE_GT(tunneledData.GetTotalCount(),0);
	
	std::cout << "Tunneled: " << tunneledData.GetTotalCount() << std::endl;

	remote.Stop();
	local.Stop();
	
	mgrTunneledMaster.Stop();
	mgrTunneledSlave.Stop();
	
	

	
}

BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
