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


#include <APL/RandomizedBuffer.h>
#include <APLTestTools/MockPhysicalLayerMonitor.h>

#include "VtoIntegrationTestBase.h"

using namespace apl;
using namespace apl::dnp;

class VtoOnewayTestStack : public VtoIntegrationTestBase
{
public:
	VtoOnewayTestStack(
	    bool clientOnSlave = true,
	    bool aImmediateOutput = false,
	    FilterLevel level = LEV_INFO,
	    boost::uint16_t port = MACRO_PORT_VALUE) :

		VtoIntegrationTestBase(clientOnSlave, aImmediateOutput, level, port),		
		local(mLog.GetLogger(level, "local-mock-phys-monitor"), &client, &timerSource, 500),
		remote(mLog.GetLogger(level, "remote-mock-phys-monitor"), &server, &timerSource, 500) 
	{

	}

	virtual ~VtoOnewayTestStack() {
		local.Stop();
		remote.Stop();
	}

	bool WaitForLocalState(PhysLayerState aState, millis_t aTimeout = 15000) {
		return testObj.ProceedUntil(boost::bind(&MockPhysicalLayerMonitor::NextStateIs, &local, aState), aTimeout);
	}

	bool WaitForRemoteState(PhysLayerState aState, millis_t aTimeout = 15000) {
		return testObj.ProceedUntil(boost::bind(&MockPhysicalLayerMonitor::NextStateIs, &remote, aState), aTimeout);
	}

	bool WaitForExpectedDataToBeReceived(millis_t aTimeout = 15000) {
		return testObj.ProceedUntil(boost::bind(&MockPhysicalLayerMonitor::AllExpectedDataHasBeenReceived, &remote), aTimeout);
	}

	MockPhysicalLayerMonitor local;
	MockPhysicalLayerMonitor remote;	
};


BOOST_AUTO_TEST_SUITE(VtoOnewayIntegrationSuite)

void TestLargeDataOneWay(VtoOnewayTestStack& arTest, size_t aSizeInBytes)
{
	// start everything	
	arTest.local.Start();
	arTest.remote.Start();
	arTest.manager.Start();
	
	BOOST_REQUIRE(arTest.WaitForLocalState(PLS_OPEN));
	BOOST_REQUIRE(arTest.WaitForRemoteState(PLS_OPEN));

	// test that a large set of data flowing one way works
	RandomizedBuffer data(aSizeInBytes);
	arTest.remote.ExpectData(data);
	arTest.local.WriteData(data);
	BOOST_REQUIRE(arTest.WaitForExpectedDataToBeReceived(15000));

	// this will cause an exception if we receive any more data beyond what we wrote
	arTest.testObj.ProceedForTime(1000);
}

BOOST_AUTO_TEST_CASE(LargeDataLoopbackMasterToSlave)
{
	VtoOnewayTestStack stack(true, false);
	TestLargeDataOneWay(stack, 500000);
}

BOOST_AUTO_TEST_CASE(LargeDataLoopbackSlaveToMaster)
{
	VtoOnewayTestStack stack(false, false);
	TestLargeDataOneWay(stack, 500000);
}

BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
