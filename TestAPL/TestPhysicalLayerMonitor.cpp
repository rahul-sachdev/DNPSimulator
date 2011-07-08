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

#include <boost/test/unit_test.hpp>

#include <APL/Log.h>
#include <APL/Exception.h>
#include <APL/PhysicalLayerMonitor.h>
#include <APLTestTools/MockTimerSource.h>
#include <APLTestTools/MockPhysicalLayerAsync.h>
#include <APLTestTools/TestHelpers.h>

using namespace apl;
using namespace boost;

class ConcretePhysicalLayerMonitor : public PhysicalLayerMonitor
{
public:

	ConcretePhysicalLayerMonitor(Logger* apLogger, IPhysicalLayerAsync* apPhys, ITimerSource* apTimerSrc) :
		Loggable(apLogger),
		PhysicalLayerMonitor(mpLogger->GetSubLogger("monitor"), apPhys, apTimerSrc, 1000) {
	}

protected:
	
	void _OnReceive(const boost::uint8_t* apData, size_t aNumBytes) {}
	void _OnSendSuccess() {}
	void _OnSendFailure() {}	
};

class TestObject
{
public:

	TestObject() :
		log(),
		mts(),
		phys(log.GetLogger(LEV_INFO, "mock-phys")),
		monitor(log.GetLogger(LEV_INFO, "test"), &phys, &mts)
	{}

	EventLog log;
	MockTimerSource mts;
	MockPhysicalLayerAsync phys;
	ConcretePhysicalLayerMonitor monitor;
};

BOOST_AUTO_TEST_SUITE(PhysicalLayerMonitorTestSuite)

BOOST_AUTO_TEST_CASE(ClosedExceptions)
{
	TestObject test;
	BOOST_CHECK_EQUAL(PLS_CLOSED, test.monitor.GetState());
	BOOST_REQUIRE_THROW(test.monitor.OnLowerLayerUp(), InvalidStateException);
}

BOOST_AUTO_TEST_CASE(StoppingWhileClosedGoesToStopped)
{
	TestObject test;
	test.monitor.Stop();
	BOOST_CHECK_EQUAL(PLS_STOPPED, test.monitor.GetState());
}

BOOST_AUTO_TEST_SUITE_END()
