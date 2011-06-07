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

#include <APL/Log.h>

#include <DNP3/VtoRouterManager.h>
#include <DNP3/VtoRouterSettings.h>
#include <DNP3/VtoWriter.h>

#include <APLTestTools/MockTimerSource.h>
#include <APLTestTools/MockPhysicalLayerSource.h>

using namespace apl;
using namespace apl::dnp;

BOOST_AUTO_TEST_SUITE(VtoRouterManagerSuite)

BOOST_AUTO_TEST_CASE(Construction)
{
	EventLog log;
	MockTimerSource mts;
	MockPhysicalLayerSource mpls(log.GetLogger(LEV_INFO, "source"));
	VtoRouterManager mgr(log.GetLogger(LEV_INFO, "test"), &mts, &mpls);	
}

BOOST_AUTO_TEST_CASE(RouterCleansUpAsynchronouslyViaDestructor)
{
	EventLog log;
	MockTimerSource mts;
	MockPhysicalLayerSource mpls(log.GetLogger(LEV_INFO, "source"), &mts);
	
	{
		VtoRouterManager mgr(log.GetLogger(LEV_INFO, "test"), &mts, &mpls);
		VtoWriter writer(100);		
		mgr.StartRouter("someport", VtoRouterSettings(0, true), &writer);
	}

	//the only way you know this test fails is if you get a memory leak warning in boost::test
	size_t num = mts.Dispatch();
}

/*
BOOST_AUTO_TEST_CASE(RouterCleansUpAsynchronouslyViaDestructor)
{
	EventLog log;
	MockTimerSource mts;
	MockPhysicalLayerSource mpls(log.GetLogger(LEV_INFO, "source"), &mts);
	
	
	VtoRouterManager mgr(log.GetLogger(LEV_INFO, "test"), &mts, &mpls);
	VtoWriter writer(100);		
	mgr.StartRouter("someport", VtoRouterSettings(0), &writer);

	


	//the only way you know this test fails is if you get a memory leak warning in boost::test
	size_t num = mts.Dispatch();
}
*/



BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
