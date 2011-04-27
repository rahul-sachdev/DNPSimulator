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

#include <APL/Log.h>
#include <APL/LogToStdio.h>

#include <boost/asio.hpp>

#include "IntegrationTest.h"

#define EXTRA_DEBUG				(0)

using namespace apl;
using namespace apl::dnp;
using namespace std;

/*
 * Each platform needs slightly different values.  Figure out what they are,
 * and use the macros in the test cases below.
 */
#if defined(WIN32)
	/* Windows platform */
	#define PORT_VALUE			(50000)
	#define NUM_PAIRS			(10)
#elif defined(ARM)
	/* Linux on ARM platform */
	#define PORT_VALUE			(30000)
	#define NUM_PAIRS			(10)
#else
	/* Generic Linux platform */
	#define PORT_VALUE			(30000)
	#define NUM_PAIRS			(100)
#endif

BOOST_AUTO_TEST_SUITE(IntegrationSuite)

BOOST_AUTO_TEST_CASE(MasterToSlave)
{
	uint_16_t port = PORT_VALUE;
	size_t numPairs = NUM_PAIRS;
	
	StopWatch sw;
	size_t numPoints = 500;
	size_t numChanges = 10;
	
	EventLog log;
	if (EXTRA_DEBUG)
		log.AddLogSubscriber(LogToStdio::Inst());

	IntegrationTest t(log.GetLogger(LEV_WARNING, "test"), LEV_WARNING, port,
			numPairs, numPoints);

	IDataObserver* pObs = t.GetFanout();

	for (size_t j = 0; j < numChanges; ++j)
	{
		/*
		 * Resource Acquisition Is Initialization (RAII) Pattern.
		 * When the Transaction instance is created, it acquires the resource.
		 * When it is destroyed, it releases the resource.  The scoping using
		 * the {} block implements this pattern.
		 */
		{
			Transaction tr(pObs);
		
			for (size_t i = 0; i < numPoints; ++i)
				pObs->Update(t.RandomBinary(), i);
		
			for (size_t i = 0; i < numPoints; ++i)
				pObs->Update(t.RandomAnalog(), i);
		
			for (size_t i = 0; i < numPoints; ++i)
				pObs->Update(t.RandomCounter(), i);
		}

		BOOST_REQUIRE(t.ProceedUntil(boost::bind(&IntegrationTest::SameData, &t)));

		if (EXTRA_DEBUG)
			cout << "***  Finished change set " <<  j << " ***" << endl;
	}

	if (EXTRA_DEBUG) {
		double elapsed_sec = sw.Elapsed() / 1000.0;
		size_t points = 3 * numPoints * numChanges * numPairs * 2;
		cout << "num points: " << points << endl;
		cout << "elapsed seconds: " << elapsed_sec << endl;
		cout << "points/sec: " << points/elapsed_sec << endl;
	}
}

BOOST_AUTO_TEST_SUITE_END()

