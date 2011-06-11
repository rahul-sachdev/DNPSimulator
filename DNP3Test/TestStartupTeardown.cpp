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
#include <APLTestTools/TestHelpers.h>

#include "StartupTeardownTest.h"

#include <boost/asio.hpp>

using namespace std;
using namespace apl;
using namespace apl::dnp;

void Configure(StartupTeardownTest& arTest, FilterLevel aLevel, boost::uint16_t aNumStacks, boost::uint16_t aNumPorts)
{
	for (boost::uint16_t i = 0; i < aNumPorts; ++i) {
		ostringstream port;
		port << "port" << i;

		/*
		 * If aAutoRun == true, the stack starts executing as soon as the
		 * first stack is added.  Otherwise, the stack will wait until
		 * Start() has been called below.
		 */
		arTest.CreatePort(port.str(), aLevel);

		for(boost::uint16_t i = 0; i < aNumStacks; ++i) {
			ostringstream stack;
			stack << port.str() << " - stack" << i;

			arTest.AddMaster(stack.str(), port.str(), i, aLevel);
		}
	}
}

BOOST_AUTO_TEST_SUITE(StartupTeardownSuite)

const FilterLevel LEVEL = LEV_WARNING;
const boost::uint16_t NUM_STACKS = 10;
const boost::uint16_t NUM_PORTS = 10;

BOOST_AUTO_TEST_CASE(NeverStart)
{
	StartupTeardownTest test(LEVEL, false);
	Configure(test, LEVEL, NUM_STACKS, NUM_PORTS);
}

BOOST_AUTO_TEST_CASE(AutoStartAndStopWithDestructor)
{
	StartupTeardownTest test(LEVEL, true);
	Configure(test, LEVEL, NUM_STACKS, NUM_PORTS);
}

BOOST_AUTO_TEST_CASE(ManualStartAndStopWithDestructor)
{
	StartupTeardownTest test(LEVEL, false);
	Configure(test, LEVEL, NUM_STACKS, NUM_PORTS);
	test.mMgr.Start();
}

BOOST_AUTO_TEST_CASE(ManualStartStop)
{
	StartupTeardownTest test(LEVEL, false);
	Configure(test, LEVEL, NUM_STACKS, NUM_PORTS);
	test.mMgr.Start();
	test.mMgr.Stop();
}

BOOST_AUTO_TEST_SUITE_END()

