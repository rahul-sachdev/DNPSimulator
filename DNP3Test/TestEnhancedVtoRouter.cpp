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
#include <APL/ToHex.h>

#include <APLTestTools/LogTester.h>
#include <APLTestTools/MockPhysicalLayerAsync.h>
#include <APLTestTools/MockTimerSource.h>
#include <APLTestTools/TestHelpers.h>

#include <DNP3/EnhancedVtoRouter.h>
#include <DNP3/VtoRouterSettings.h>
#include <DNP3/VtoWriter.h>

using namespace std;
using namespace apl;
using namespace apl::dnp;

class ServerVtoRouterTestClass : LogTester
{
public:
	ServerVtoRouterTestClass(const VtoRouterSettings& arSettings = VtoRouterSettings(88, true, true), const size_t aWriterSize = 100) :
		LogTester(false),
		phys(mLog.GetLogger(LEV_DEBUG, "phys")),
		writer(aWriterSize),
		mts(),
		router(arSettings, mLog.GetLogger(LEV_DEBUG, "router"), &writer, &phys, &mts) {
		writer.AddVtoCallback(&router);
	}

	void CheckLocalChannelConnectedMessage(bool connected) {
		BOOST_REQUIRE_EQUAL(writer.Size(), 1);
		VtoEvent vto;
		BOOST_REQUIRE(writer.Read(vto));
		BOOST_REQUIRE_EQUAL(vto.mValue.mpData[0], 88);
		BOOST_REQUIRE_EQUAL(vto.mValue.mpData[1], (connected ? 0 : 1));
	}

	MockPhysicalLayerAsync phys;
	VtoWriter writer;
	MockTimerSource mts;
	ServerSocketVtoRouter router;
};

class ClientVtoRouterTestClass : LogTester
{
public:
	ClientVtoRouterTestClass(const VtoRouterSettings& arSettings = VtoRouterSettings(88, true, true), const size_t aWriterSize = 100) :
		LogTester(false),
		phys(mLog.GetLogger(LEV_DEBUG, "phys")),
		writer(aWriterSize),
		mts(),
		router(arSettings, mLog.GetLogger(LEV_DEBUG, "router"), &writer, &phys, &mts) {
		writer.AddVtoCallback(&router);
	}

	void CheckLocalChannelConnectedMessage(bool connected) {
		BOOST_REQUIRE_EQUAL(writer.Size(), 1);
		VtoEvent vto;
		BOOST_REQUIRE(writer.Read(vto));
		BOOST_REQUIRE_EQUAL(vto.mValue.mpData[0], 88);
		BOOST_REQUIRE_EQUAL(vto.mValue.mpData[1], (connected ? 0 : 1));
	}

	MockPhysicalLayerAsync phys;
	VtoWriter writer;
	MockTimerSource mts;
	ClientSocketVtoRouter router;
};

BOOST_AUTO_TEST_SUITE(EnhancedVtoRouterTests)

BOOST_AUTO_TEST_CASE(Construction)
{
	ServerVtoRouterTestClass rtc;
}

BOOST_AUTO_TEST_CASE(ServerStartsOpeningAfterDnpConnection)
{
	ServerVtoRouterTestClass rtc;
	BOOST_REQUIRE(!rtc.phys.IsOpening());

	rtc.router.OnDnpConnectedChanged(true);

	rtc.mts.Dispatch();
	BOOST_REQUIRE(rtc.phys.IsOpening());

	rtc.router.OnDnpConnectedChanged(false);

	rtc.mts.Dispatch();
	BOOST_REQUIRE(rtc.phys.IsClosing());
}

BOOST_AUTO_TEST_CASE(ServerSendsMagicChannelLocalConnected)
{
	ServerVtoRouterTestClass rtc;

	rtc.router.OnDnpConnectedChanged(true);
	rtc.mts.Dispatch();
	BOOST_REQUIRE(rtc.phys.IsOpening());
	rtc.phys.SignalOpenSuccess();

	rtc.CheckLocalChannelConnectedMessage(true);

	rtc.phys.TriggerClose();
	rtc.mts.Dispatch();

	BOOST_REQUIRE_EQUAL(rtc.phys.NumClose(), 1);

	rtc.CheckLocalChannelConnectedMessage(false);
}

BOOST_AUTO_TEST_CASE(ClientStartsOpeningAfterRemoteConnection)
{
	ClientVtoRouterTestClass rtc;
	BOOST_REQUIRE(!rtc.phys.IsOpening());

	rtc.router.OnDnpConnectedChanged(true);

	BOOST_REQUIRE(!rtc.phys.IsOpening());

	rtc.router.OnVtoRemoteConnectedChanged(true);

	rtc.mts.Dispatch();
	BOOST_REQUIRE(rtc.phys.IsOpening());

	rtc.router.OnDnpConnectedChanged(false);

	rtc.mts.Dispatch();
	BOOST_REQUIRE(rtc.phys.IsClosing());
}

BOOST_AUTO_TEST_CASE(ClientSendsMagicChannelLocalConnected)
{
	ClientVtoRouterTestClass rtc;

	rtc.router.OnDnpConnectedChanged(true);
	rtc.router.OnVtoRemoteConnectedChanged(true);
	rtc.mts.Dispatch();
	BOOST_REQUIRE(rtc.phys.IsOpening());
	rtc.phys.SignalOpenSuccess();

	rtc.CheckLocalChannelConnectedMessage(true);

	rtc.phys.TriggerClose();
	rtc.mts.Dispatch();

	BOOST_REQUIRE_EQUAL(rtc.phys.NumClose(), 1);

	rtc.CheckLocalChannelConnectedMessage(false);
}

BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
