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

#include <APL/PhysLoopback.h>
#include <APL/RandomizedBuffer.h>

#include <APLTestTools/MockPhysicalLayerMonitor.h>

#include "VtoIntegrationTestBase.h"

using namespace apl;
using namespace apl::dnp;

class VtoLoopbackTestStack : public VtoIntegrationTestBase
{
public:
	VtoLoopbackTestStack(
	    bool clientOnSlave = true,
	    bool aImmediateOutput = false,
	    FilterLevel level = LEV_INFO,
	    boost::uint16_t port = MACRO_PORT_VALUE) :

		VtoIntegrationTestBase(clientOnSlave, aImmediateOutput, level, port),
		loopback(mLog.GetLogger(level, "loopback"), &server, &timerSource),
		local(mLog.GetLogger(level, "mock-client-connection"), &client, &timerSource, 500) {
	}

	virtual ~VtoLoopbackTestStack() {
		local.Stop();
		loopback.Stop();
	}

	bool WaitForLocalState(PhysLayerState aState, millis_t aTimeout = 15000) {
		return testObj.ProceedUntil(boost::bind(&MockPhysicalLayerMonitor::NextStateIs, &local, aState), aTimeout);
	}

	bool WaitForExpectedDataToBeReceived(millis_t aTimeout = 15000) {
		return testObj.ProceedUntil(boost::bind(&MockPhysicalLayerMonitor::AllExpectedDataHasBeenReceived, &local), aTimeout);
	}

	PhysLoopback loopback;
	MockPhysicalLayerMonitor local;
};


BOOST_AUTO_TEST_SUITE(VtoLoopbackIntegrationSuite)

BOOST_AUTO_TEST_CASE(Reconnection)
{
	VtoLoopbackTestStack stack(true, false);

	// start up everything, the local side should be able to open
	stack.manager.Start();
	stack.loopback.Start();
	stack.local.Start();

	RandomizedBuffer data(100);

	for(size_t i = 0; i < 3; ++i) {

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
	VtoLoopbackTestStack test(true, false);

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
	VtoLoopbackTestStack stack(true, false);

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

void TestLargeDataLoopback(VtoLoopbackTestStack& arTest, size_t aSizeInBytes)
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

BOOST_AUTO_TEST_CASE(LargeDataLoopbackMasterToSlave)
{
	VtoLoopbackTestStack stack(true, false);
	TestLargeDataLoopback(stack, 500000);
}

BOOST_AUTO_TEST_CASE(LargeDataLoopbackSlaveToMaster)
{
	VtoLoopbackTestStack stack(false, false);
	TestLargeDataLoopback(stack, 500000);
}

BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
