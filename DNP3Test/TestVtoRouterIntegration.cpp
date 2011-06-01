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
	const FilterLevel level = LEV_INFO;

	EventLog log;
	FlexibleDataObserver fdo;
	MockCommandAcceptor cmdAcceptor;	
	AsyncStackManager mgr(log.GetLogger(level, "test"));
	
	mgr.AddTCPClient("client", PhysLayerSettings(), "127.0.0.1", port);
	mgr.AddTCPServer("server", PhysLayerSettings(), "127.0.0.1", port);

	mgr.AddSlave("server", "slave", level, &cmdAcceptor, SlaveStackConfig());
	mgr.AddMaster("client", "master", level, &fdo, MasterStackConfig());
}

BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
