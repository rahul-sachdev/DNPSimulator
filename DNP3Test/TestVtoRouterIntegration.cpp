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
#include <APLTestTools/AsyncTestObjectASIO.h>
#include <APLTestTools/BufferHelpers.h>
#include <APLTestTools/LogTester.h>
#include <APLTestTools/MockPhysicalLayerMonitor.h>

#include <APL/Log.h>
#include <APL/LogToStdio.h>
#include <APL/LogToFile.h>
#include <APL/FlexibleDataObserver.h>
#include <APL/AsyncPhysLayerMonitor.h>
#include <APL/PhysLoopback.h>
#include <APL/RandomizedBuffer.h>

#include <DNP3/AsyncStackManager.h>
#include <DNP3/SlaveStackConfig.h>
#include <DNP3/MasterStackConfig.h>
#include <DNP3/VtoRouterSettings.h>

#include <APL/PhysicalLayerAsyncTCPClient.h>
#include <APL/PhysicalLayerAsyncTCPServer.h>

using namespace apl;
using namespace apl::dnp;

/** Platforms have different reserved port ranges */

#if defined(WIN32)
/* Windows platform */
#define MACRO_PORT_VALUE	(50000)
#else
/* Generic Linux platform */
#define MACRO_PORT_VALUE	(30000)
#endif

class VtoTestStack : public LogTester
{
public:
	VtoTestStack(
	    bool clientOnSlave = true,
	    bool aImmediateOutput = false,
	    FilterLevel level = LEV_INFO,
	    boost::uint16_t port = MACRO_PORT_VALUE) :

		LogTester(),
		mpMainLogger(mLog.GetLogger(level, "main")),
		ltf(&mLog, "integration.log", true),
		testObj(),
		manager(mLog.GetLogger(level, "manager"), false),
		timerSource(testObj.GetService()),
		client(mLog.GetLogger(level, "local-tcp-client"), testObj.GetService(), "127.0.0.1", port + 20),
		server(mLog.GetLogger(level, "loopback-tcp-server"), testObj.GetService(), "0.0.0.0", port + 10),
		loopback(mLog.GetLogger(level, "loopback"), &server, &timerSource),
		local(mLog.GetLogger(level, "mock-client-connection"), &client, &timerSource, 500) {

		if(aImmediateOutput) mLog.AddLogSubscriber(LogToStdio::Inst());

		manager.AddTCPServer("dnp-tcp-server", PhysLayerSettings(), "127.0.0.1", port);
		manager.AddSlave("dnp-tcp-server", "slave", level, &cmdAcceptor, SlaveStackConfig());

		manager.AddTCPClient("dnp-tcp-client", PhysLayerSettings(), "127.0.0.1", port);
		manager.AddMaster("dnp-tcp-client", "master", level, &fdo, MasterStackConfig());

		// switch if master or slave gets the loopback half of the server

		std::string clientSideOfStack = clientOnSlave ? "slave" : "master";
		std::string serverSideOfStack = clientOnSlave ? "master" : "slave";

		manager.AddTCPClient("vto-tcp-client", PhysLayerSettings(), "127.0.0.1", port + 10);
		manager.StartVtoRouter("vto-tcp-client", clientSideOfStack, VtoRouterSettings(88, false, false, 4096, 1000));
		manager.AddTCPServer("vto-tcp-server", PhysLayerSettings(), "127.0.0.1", port + 20);
		manager.StartVtoRouter("vto-tcp-server", serverSideOfStack, VtoRouterSettings(88, true, false, 4096, 1000));
	}

	~VtoTestStack() {
		manager.Stop();
		local.Stop();
		loopback.Stop();
	}

	bool WaitForLocalState(PhysLayerState aState, millis_t aTimeout = 15000) {
		return testObj.ProceedUntil(boost::bind(&MockPhysicalLayerMonitor::NextStateIs, &local, aState), aTimeout);
	}

	bool WaitForExpectedDataToBeReceived(millis_t aTimeout = 15000) {
		return testObj.ProceedUntil(boost::bind(&MockPhysicalLayerMonitor::AllExpectedDataHasBeenReceived, &local), aTimeout);
	}

	Logger* mpMainLogger;
	LogToFile ltf;
	FlexibleDataObserver fdo;
	MockCommandAcceptor cmdAcceptor;

	AsyncTestObjectASIO testObj;
	AsyncStackManager manager;

	TimerSourceASIO timerSource;
	PhysicalLayerAsyncTCPClient client;
	PhysicalLayerAsyncTCPServer server;

	PhysLoopback loopback;

	MockPhysicalLayerMonitor local;
};


BOOST_AUTO_TEST_SUITE(VtoRouterIntegrationSuite)

BOOST_AUTO_TEST_CASE(Reconnection)
{
	VtoTestStack stack(true, false);	

	// start up everything, the local side should be able to open
	stack.manager.Start();
	stack.loopback.Start();
	stack.local.Start();
	
	RandomizedBuffer data(100);

	for(size_t i=0; i<3; ++i) {
			
		BOOST_REQUIRE(stack.WaitForLocalState(PLS_OPEN));
	
		// test that data is correctly sent both ways
		data.Randomize();
		stack.local.ExpectData(data);
		stack.local.WriteData(data);
		BOOST_REQUIRE(stack.WaitForExpectedDataToBeReceived());

		// stop the remote loopback server, which should cause the local vto socket to close and reopen
		stack.loopback.Stop();
		BOOST_REQUIRE(stack.WaitForLocalState(PLS_CLOSED));
		stack.loopback.Start();		
	}
}

BOOST_AUTO_TEST_CASE(RemoteSideOpenFailureBouncesLocalConnection)
{
	VtoTestStack test(true, false);

	BOOST_REQUIRE(test.WaitForLocalState(PLS_CLOSED));

	test.manager.Start();
	test.local.Start();

	for(size_t i = 0; i < 5; ++i) {
		// start local connection, we should immediately be able to connect to this side
		BOOST_REQUIRE(test.WaitForLocalState(PLS_OPEN));
		// since the remote side can't connect to the port we should have our local connection bounced
		BOOST_REQUIRE(test.WaitForLocalState(PLS_CLOSED));
	}
}

BOOST_AUTO_TEST_CASE(SocketIsClosedIfRemoteDrops)
{
	VtoTestStack stack(true, false);

	// start all 4 components, should connect
	stack.manager.Start();
	stack.loopback.Start();
	stack.local.Start();

	BOOST_REQUIRE(stack.WaitForLocalState(PLS_OPEN));

	// kill remote connection, should kill our local connection
	stack.mpMainLogger->Log(LEV_EVENT, LOCATION, "Stopping loopback");
	stack.loopback.Stop();
	stack.mpMainLogger->Log(LEV_EVENT, LOCATION, "Stopped loopback");

	BOOST_REQUIRE(stack.WaitForLocalState(PLS_CLOSED));	
}

void TestLargeDataTransmission(VtoTestStack& arTest, size_t aSizeInBytes)
{
	// start everything
	arTest.manager.Start();
	arTest.loopback.Start();
	arTest.local.Start();
	BOOST_REQUIRE(arTest.WaitForLocalState(PLS_OPEN));

	// test that a large set of data flowing one way works
	RandomizedBuffer data(aSizeInBytes);
	arTest.local.ExpectData(data);
	arTest.local.WriteData(data);
	BOOST_REQUIRE(arTest.WaitForExpectedDataToBeReceived(15000));
	
	// this will cause an exception if we receive any more data beyond what we wrote
	arTest.testObj.ProceedForTime(1000);
}

BOOST_AUTO_TEST_CASE(LargeDataTransmissionMasterToSlave)
{
	VtoTestStack stack(true, false);
	TestLargeDataTransmission(stack, 500000);
}

BOOST_AUTO_TEST_CASE(LargeDataTransmissionSlaveToMaster)
{
	VtoTestStack stack(false, false);
	TestLargeDataTransmission(stack, 500000);
}

BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
