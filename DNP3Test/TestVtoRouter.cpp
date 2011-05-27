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
#include <APLTestTools/LogTester.h>
#include <APLTestTools/MockPhysicalLayerAsync.h>
#include <APLTestTools/MockTimerSource.h>
#include <APLTestTools/TestHelpers.h>

#include <DNP3/VtoRouter.h>
#include <DNP3/VtoWriter.h>

using namespace std;
using namespace apl;
using namespace apl::dnp;

class RouterTestClass : LogTester 
{
	public:
	RouterTestClass() :
		LogTester(false),
		phys(mLog.GetLogger(LEV_DEBUG, "phys")),
		writer(100),
		mts(),
		router(mLog.GetLogger(LEV_DEBUG, "router"), 8, &writer, &phys, &mts)
	{}

	MockPhysicalLayerAsync phys;
	VtoWriter writer;
	MockTimerSource mts;
	VtoRouter router;
};

BOOST_AUTO_TEST_SUITE(VtoRouterTests)

	BOOST_AUTO_TEST_CASE(Construction)
	{
		RouterTestClass rtc;
	}

	BOOST_AUTO_TEST_CASE(StartsReadingAfterOpen)
	{
		RouterTestClass rtc;		
		rtc.router.Start();
		BOOST_REQUIRE(rtc.phys.IsOpening());
		rtc.phys.SignalOpenSuccess();
		BOOST_REQUIRE(rtc.phys.IsReading());
	}

BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
