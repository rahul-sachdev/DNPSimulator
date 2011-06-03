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

BOOST_AUTO_TEST_CASE(MasterToSlave)
{
	const boost::uint16_t port = PORT_VALUE;
	const FilterLevel level = LEV_WARNING;

	EventLog log;
	log.AddLogSubscriber(LogToStdio::Inst());
	FlexibleDataObserver fdo;
	FlexibleDataObserver tunneledData;
	MockCommandAcceptor cmdAcceptor;
	AsyncStackManager mgr(log.GetLogger(level, "test"));
	
	mgr.AddTCPClient("client", PhysLayerSettings(), "127.0.0.1", port);
	mgr.AddTCPServer("server", PhysLayerSettings(), "127.0.0.1", port);
	
	mgr.AddSlave("client", "slave", level, &cmdAcceptor, SlaveStackConfig());
	mgr.AddMaster("server", "master", level, &fdo, MasterStackConfig());
	
	mgr.AddTCPServer("vtoserver1", PhysLayerSettings(), "127.0.0.1", port+10);
	mgr.AddTCPClient("vtoserver2", PhysLayerSettings(), "127.0.0.1", port+20);
	
	mgr.StartVtoRouter("vtoserver1", "master", VtoRouterSettings(0));
	mgr.StartVtoRouter("vtoserver2", "slave", VtoRouterSettings(0));

	mgr.AddTCPClient("tunneled_client", PhysLayerSettings(), "127.0.0.1", port+10);
	mgr.AddTCPServer("tunneled_server", PhysLayerSettings(), "127.0.0.1", port+20);

	mgr.AddSlave("tunneled_server", "tunneled_slave", level, &cmdAcceptor, SlaveStackConfig());
	mgr.AddMaster("tunneled_client", "tunneled_master", level, &tunneledData, MasterStackConfig());
	
	BOOST_REQUIRE_EQUAL(tunneledData.GetTotalCount(),0);

	mgr.Start();

	Thread::SleepFor(1000);

	BOOST_REQUIRE_GE(tunneledData.GetTotalCount(),0);

	mgr.Stop();
}

BOOST_AUTO_TEST_CASE(SlaveToMaster)
{
	const boost::uint16_t port = PORT_VALUE;
	const FilterLevel level = LEV_WARNING;

	EventLog log;
	log.AddLogSubscriber(LogToStdio::Inst());
	FlexibleDataObserver fdo;
	FlexibleDataObserver tunneledData;
	MockCommandAcceptor cmdAcceptor;
	AsyncStackManager mgr(log.GetLogger(level, "test"));
	
	mgr.AddTCPClient("client", PhysLayerSettings(), "127.0.0.1", port);
	mgr.AddTCPServer("server", PhysLayerSettings(), "127.0.0.1", port);
	
	mgr.AddSlave("client", "slave", level, &cmdAcceptor, SlaveStackConfig());
	mgr.AddMaster("server", "master", level, &fdo, MasterStackConfig());
	
	mgr.AddTCPServer("vtoserver1", PhysLayerSettings(), "127.0.0.1", port+10);
	mgr.AddTCPClient("vtoserver2", PhysLayerSettings(), "127.0.0.1", port+20);
	
	mgr.StartVtoRouter("vtoserver1", "master", VtoRouterSettings(0));
	mgr.StartVtoRouter("vtoserver2", "slave", VtoRouterSettings(0));

	mgr.AddTCPClient("tunneled_client", PhysLayerSettings(), "127.0.0.1", port+10);
	mgr.AddTCPServer("tunneled_server", PhysLayerSettings(), "127.0.0.1", port+20);

	mgr.AddSlave("tunneled_server", "tunneled_slave", level, &cmdAcceptor, SlaveStackConfig());
	mgr.AddMaster("tunneled_client", "tunneled_master", level, &tunneledData, MasterStackConfig());
	
	BOOST_REQUIRE_EQUAL(tunneledData.GetTotalCount(),0);

	mgr.Start();

	Thread::SleepFor(1000);

	BOOST_REQUIRE_GE(tunneledData.GetTotalCount(),0);

	mgr.Stop();
}
/*
BOOST_AUTO_TEST_CASE(MultiThreadedSlaveToMaster)
{
	const boost::uint16_t port = PORT_VALUE;
	const FilterLevel level = LEV_WARNING;

	EventLog log;
	log.AddLogSubscriber(LogToStdio::Inst());
	FlexibleDataObserver fdo;
	FlexibleDataObserver tunneledData;
	MockCommandAcceptor cmdAcceptor;
	AsyncStackManager mgr(log.GetLogger(level, "test"));
	
	mgr.AddTCPClient("client", PhysLayerSettings(), "127.0.0.1", port);
	mgr.AddTCPServer("server", PhysLayerSettings(), "127.0.0.1", port);
	
	mgr.AddSlave("client", "master", level, &cmdAcceptor, SlaveStackConfig());
	mgr.AddMaster("server", "slave", level, &fdo, MasterStackConfig());
	
	mgr.AddTCPServer("vtoserver1", PhysLayerSettings(), "127.0.0.1", port+10);
	mgr.AddTCPClient("vtoserver2", PhysLayerSettings(), "127.0.0.1", port+20);
	
	mgr.StartVtoRouter("vtoserver1", "master", VtoRouterSettings(0));
	mgr.StartVtoRouter("vtoserver2", "slave", VtoRouterSettings(0));

	AsyncStackManager mgrTunneled(log.GetLogger(level, "tunneled"));

	mgrTunneled.AddTCPClient("tunneled_client", PhysLayerSettings(), "127.0.0.1", port+10);
	mgrTunneled.AddTCPServer("tunneled_server", PhysLayerSettings(), "127.0.0.1", port+20);

	mgrTunneled.AddSlave("tunneled_server", "tunneled_slave", level, &cmdAcceptor, SlaveStackConfig());
	mgrTunneled.AddMaster("tunneled_client", "tunneled_master", level, &tunneledData, MasterStackConfig());
	
	mgr.Start();

	BOOST_REQUIRE_EQUAL(tunneledData.GetTotalCount(),0);

	mgrTunneled.Start();

	Thread::SleepFor(1000);

	BOOST_REQUIRE_GE(tunneledData.GetTotalCount(),0);

	mgrTunneled.Stop();
	mgr.Stop();
	
}*/

BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
