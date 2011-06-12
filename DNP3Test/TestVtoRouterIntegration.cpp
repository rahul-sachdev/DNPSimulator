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
#include <APLTestTools/AsyncPhysTestObject.h>
#include <APLTestTools/BufferHelpers.h>

#include <APL/Log.h>
#include <APL/LogToStdio.h>
#include <APL/LogToFile.h>
#include <APL/FlexibleDataObserver.h>
#include <APL/AsyncPhysLayerMonitor.h>

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
#define PORT_VALUE			(50000)
#else
/* Generic Linux platform */
#define PORT_VALUE			(30000)
#endif

class MockClientConnection : public AsyncPhysLayerMonitor
{
public:
	MockClientConnection(Logger* apLogger, IPhysicalLayerAsync* apPhys, ITimerSource* apTimer, millis_t aOpenRetry) :
		Loggable(apLogger),
		AsyncPhysLayerMonitor(apLogger, apPhys, apTimer, aOpenRetry),
		mOpens(0),
		mCloses(0),
		mOpenFailures(0),
		mRead(1024),
		mLastRead(0) {
		mState.push(IPhysMonitor::Closed);
	}

	size_t mOpens;
	size_t mCloses;
	size_t mOpenFailures;

	CopyableBuffer mRead;
	CopyableBuffer mLastRead;

	std::queue< IPhysMonitor::State > mState;

	void OnPhysicalLayerOpen() {
		mOpens++;
		mpPhys->AsyncRead(mRead, mRead.Size());
	}
	void OnPhysicalLayerClose() {
		mCloses++;
	}

	void OnPhysicalLayerOpenFailure() {
		mOpenFailures++;
	}

	void OnStateChange(IPhysMonitor::State aState) {
		LOG_BLOCK(LEV_INFO, "State: " << aState);
		mState.push(aState);
	}

	void _OnReceive(const boost::uint8_t* apData, size_t aNumBytes) {
		CopyableBuffer cb(apData, aNumBytes);
		mLastRead = cb;
		mpPhys->AsyncRead(mRead, mRead.Size());
	}

	void WriteData(CopyableBuffer& aData) {
		mpPhys->AsyncWrite(aData, aData.Size());
	}

	void _OnSendSuccess(void) {}
	void _OnSendFailure(void) {}

	bool StateIs(IPhysMonitor::State aState) {
		if(mState.empty()) return false;
		bool matchedState = mState.front() == aState;
		if(mState.size() > 1) mState.pop();
		return matchedState;
	}
	bool CountsAre(size_t aOpens, size_t aCloses, size_t aOpenFails) {
		return mOpens == aOpens && mCloses == aCloses && mOpenFailures == aOpenFails;
	}
	bool DataIs(CopyableBuffer& cb) {
		return cb == mLastRead;
	}

};

class VtoTestStack
{
public:
	VtoTestStack(FilterLevel level = LEV_INFO, boost::uint16_t port = PORT_VALUE) :
		main(log.GetLogger(level, "main")),
		ltf(&log, "integration.log", true),
		slaveMgr(log.GetLogger(level, "slave"), false),
		masterMgr(log.GetLogger(level, "master"), false),
		timerSource(testObj.GetService()),
		client(log.GetLogger(level, "local"), testObj.GetService(), "127.0.0.1", port + 20),
		server(log.GetLogger(level, "remote"), testObj.GetService(), "0.0.0.0", port + 10),
		loopback(log.GetLogger(level, "loopback"), &server, &timerSource),
		local(log.GetLogger(level, "mock"), &client, &timerSource, 50) {
		//log.AddLogSubscriber(LogToStdio::Inst());

		slaveMgr.AddTCPServer("dnp_server", PhysLayerSettings(), "127.0.0.1", port);
		slaveMgr.AddTCPClient("vto_client", PhysLayerSettings(), "127.0.0.1", port + 10);
		slaveMgr.AddSlave("dnp_server", "slave", level, &cmdAcceptor, SlaveStackConfig());
		slaveMgr.StartVtoRouter("vto_client", "slave", VtoRouterSettings(88, false, false, 4096, 1000));

		masterMgr.AddTCPClient("dnp_client", PhysLayerSettings(), "127.0.0.1", port);
		masterMgr.AddTCPServer("vto_server", PhysLayerSettings(), "127.0.0.1", port + 20);
		masterMgr.AddMaster("dnp_client", "master", level, &fdo, MasterStackConfig());
		masterMgr.StartVtoRouter("vto_server", "master", VtoRouterSettings(88, true, false, 4096, 1000));
	}

	~VtoTestStack() {
		masterMgr.Stop();
		slaveMgr.Stop();

		local.Stop();
		loopback.Stop();
	}

	bool checkState(IPhysMonitor::State aState) {
		return testObj.ProceedUntil(boost::bind(&MockClientConnection::StateIs, &local, aState), 10000);
	}
	bool checkData(CopyableBuffer& cb) {
		return testObj.ProceedUntil(boost::bind(&MockClientConnection::DataIs, &local, cb), 10000);
	}

	EventLog log;

	Logger* main;
	LogToFile ltf;
	FlexibleDataObserver fdo;
	MockCommandAcceptor cmdAcceptor;

	AsyncTestObjectASIO testObj;
	AsyncStackManager slaveMgr;
	AsyncStackManager masterMgr;

	TimerSourceASIO timerSource;
	PhysicalLayerAsyncTCPClient client;
	PhysicalLayerAsyncTCPServer server;

	AsyncLoopback loopback;

	MockClientConnection local;
};


BOOST_AUTO_TEST_SUITE(VtoRouterIntegrationSuite)

BOOST_AUTO_TEST_CASE(Reconnect)
{
	VtoTestStack stack;

	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Closed));

	// start up both halfs of the dnp3 connection, vto server port will be online
	stack.masterMgr.Start();
	stack.slaveMgr.Start();
	// startup the loopback server on the slave side
	stack.loopback.Start();

	// startup the local connection to the vto socket, since the whole stack is up we should connect
	stack.local.Start();
	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Open));

	// test that data is correctly sent both ways
	HexSequence testData1("de ad be ef");
	stack.local.WriteData(testData1);
	BOOST_REQUIRE(stack.checkData(testData1));

	// stop the remote loopback server, which should cause the local vto socket to close and reopen
	stack.loopback.Stop();
	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Closed));

	// check that we can reconnect to the loopback if it is turned back on
	stack.loopback.Start();
	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Open));

	// send a second set of data
	HexSequence testData2("01 02 03 04");
	stack.local.WriteData(testData2);
	BOOST_REQUIRE(stack.checkData(testData2));

	// disconnect again
	stack.loopback.Stop();
	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Closed));
}

BOOST_AUTO_TEST_CASE(ServerNotOpenUntilDnpConnected)
{
	VtoTestStack stack;

	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Closed));

	// start master side, since there is no dnp connection to slave we wont start listening socket
	stack.masterMgr.Start();

	// start local connection, we wont be able to connect to vto socket because there is no dnp3 connection made yet so we should
	// end up in the waiting state
	stack.local.Start();
	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Waiting));

	// start the slave side of the dnp3 connection and verify that we can connect to the vto server
	stack.slaveMgr.Start();
	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Open));

	// since the remote side can't connect to the port we should have our local connection bounced
	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Closed));
}

BOOST_AUTO_TEST_CASE(SocketIsClosedIfDnpDrops)
{
	VtoTestStack stack;

	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Closed));

	// start all 4 components, should connect
	stack.masterMgr.Start();
	stack.slaveMgr.Start();
	stack.loopback.Start();
	stack.local.Start();

	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Open));

	// kill slave dnp3, should kill our local connection
	stack.slaveMgr.Stop();

	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Closed));
}

BOOST_AUTO_TEST_CASE(SocketIsClosedIfRemoteDrops)
{
	VtoTestStack stack;

	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Closed));

	// start all 4 components, should connect
	stack.masterMgr.Start();
	stack.slaveMgr.Start();
	stack.loopback.Start();
	stack.local.Start();

	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Open));

	// kill remote connection, should kill our local connection
	stack.loopback.Stop();

	BOOST_REQUIRE(stack.checkState(IPhysMonitor::Closed));
}

BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
