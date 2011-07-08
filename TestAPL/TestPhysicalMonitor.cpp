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
#include <APL/AsyncPhysLayerMonitor.h>
#include <APLTestTools/MockTimerSource.h>
#include <APLTestTools/MockPhysicalLayerAsync.h>
#include <APLTestTools/TestHelpers.h>

using namespace apl;
using namespace boost;

BOOST_AUTO_TEST_SUITE(PhysicalLayerMonitorSuite)

class TestPhysicalLayerMonitor : public AsyncPhysLayerMonitor
{
public:

	TestPhysicalLayerMonitor(Logger* apLogger, IPhysicalLayerAsync* apPhys, ITimerSource* apTimerSrc) :
		Loggable(apLogger),
		AsyncPhysLayerMonitor(mpLogger->GetSubLogger("monitor"), apPhys, apTimerSrc, 1000) {
	}

protected:

	void OnPhysicalLayerOpen() {}
	void OnPhysicalLayerClose() {}
	void OnPhysicalLayerOpenFailure() {}
	void OnStateChange(PhysicalLayerState) {}


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
	TestPhysicalLayerMonitor monitor;
};



BOOST_AUTO_TEST_CASE(Construction)
{
	TestObject test;
	BOOST_CHECK_EQUAL(PLS_STOPPED, test.monitor.GetState());
}

BOOST_AUTO_TEST_CASE(StartIsIdempotent)
{
	TestObject test;
	for(size_t i = 0; i < 3; ++i) {
		test.monitor.Start();
		BOOST_CHECK_EQUAL(PLS_OPENING, test.monitor.GetState());
		BOOST_REQUIRE(test.phys.IsOpening());
	}
}

BOOST_AUTO_TEST_CASE(StopIsIdempotent)
{
	TestObject test;
	for(size_t i = 0; i < 3; ++i) {
		test.monitor.Stop();
		BOOST_CHECK_EQUAL(PLS_STOPPED, test.monitor.GetState());
	}
}

BOOST_AUTO_TEST_CASE(RetriesOpenFailures)
{
	TestObject test;
	test.monitor.Start();

	for(size_t i = 0; i < 3; ++i) {
		BOOST_CHECK_EQUAL(PLS_OPENING, test.monitor.GetState());
		test.phys.SignalOpenFailure();
		BOOST_CHECK_EQUAL(PLS_WAITING, test.monitor.GetState());
		BOOST_REQUIRE_EQUAL(1, test.mts.NumActive());
		BOOST_REQUIRE(test.mts.DispatchOne());
	}
}

BOOST_AUTO_TEST_CASE(StopWhileOpeningDefersStateTransition)
{
	TestObject test;
	test.monitor.Start();
	BOOST_CHECK_EQUAL(PLS_OPENING, test.monitor.GetState());
	test.monitor.Stop();
	BOOST_CHECK_EQUAL(PLS_OPENING, test.monitor.GetState());
	test.phys.SignalOpenFailure();
	BOOST_CHECK_EQUAL(PLS_STOPPED, test.monitor.GetState());
}

BOOST_AUTO_TEST_SUITE_END()
