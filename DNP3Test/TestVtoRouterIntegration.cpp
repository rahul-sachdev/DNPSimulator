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
#include <APLTestTools/AsyncLoopback.h>
#include <APLTestTools/BufferHelpers.h>
#include <APLTestTools/LogTester.h>

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
		mState.push(PLS_CLOSED);
	}

	size_t mOpens;
	size_t mCloses;
	size_t mOpenFailures;

	CopyableBuffer mRead;
	CopyableBuffer mLastRead;

	std::queue< PhysLayerState > mState;

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

	void OnStateChange(PhysLayerState aState) {
		LOG_BLOCK(LEV_INFO, "OnStateChange: " << ConvertToString(aState));
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

	bool StateIs(PhysLayerState aState) {
		if(mState.empty()) return false;
		else {
			bool matchedState = mState.front() == aState;
			mState.pop();
			return matchedState;
		}
	}
	bool CountsAre(size_t aOpens, size_t aCloses, size_t aOpenFails) {
		return mOpens == aOpens && mCloses == aCloses && mOpenFailures == aOpenFails;
	}
	bool DataIs(CopyableBuffer& cb) {
		return cb == mLastRead;
	}

};

class VtoTestStack : public LogTester
{
public:
	VtoTestStack(FilterLevel level = LEV_INFO, boost::uint16_t port = PORT_VALUE) :
		LogTester(),
		mpMainLogger(mLog.GetLogger(level, "main")),
		ltf(&mLog, "integration.log", true),
		slaveMgr(mLog.GetLogger(level, "slave"), false),
		masterMgr(mLog.GetLogger(level, "master"), false),
		timerSource(testObj.GetService()),
		client(mLog.GetLogger(level, "local-tcp-client"), testObj.GetService(), "127.0.0.1", port + 20),
		server(mLog.GetLogger(level, "remote-tcp-server"), testObj.GetService(), "0.0.0.0", port + 10),
		loopback(mLog.GetLogger(level, "loopback"), &server, &timerSource),
		local(mLog.GetLogger(level, "mock-client-connection"), &client, &timerSource, 500) {

		//mLog.AddLogSubscriber(LogToStdio::Inst());

		slaveMgr.AddTCPServer("dnp-tcp-server", PhysLayerSettings(), "127.0.0.1", port);
		slaveMgr.AddTCPClient("vto-tcp-client", PhysLayerSettings(), "127.0.0.1", port + 10);
		slaveMgr.AddSlave("dnp-tcp-server", "slave", level, &cmdAcceptor, SlaveStackConfig());
		slaveMgr.StartVtoRouter("vto-tcp-client", "slave", VtoRouterSettings(88, false, false, 4096, 1000));

		masterMgr.AddTCPClient("dnp-tcp-client", PhysLayerSettings(), "127.0.0.1", port);
		masterMgr.AddTCPServer("vto-tcp-server", PhysLayerSettings(), "127.0.0.1", port + 20);
		masterMgr.AddMaster("dnp-tcp-client", "master", level, &fdo, MasterStackConfig());
		masterMgr.StartVtoRouter("vto-tcp-server", "master", VtoRouterSettings(88, true, false, 4096, 1000));
	}

	~VtoTestStack() {
		masterMgr.Stop();
		slaveMgr.Stop();

		local.Stop();
		loopback.Stop();
	}

	bool WaitForState(PhysLayerState aState) {
		return testObj.ProceedUntil(boost::bind(&MockClientConnection::StateIs, &local, aState), 10000);
	}

	bool WaitForData(CopyableBuffer& cb) {
		return testObj.ProceedUntil(boost::bind(&MockClientConnection::DataIs, &local, cb), 10000);
	}

	Logger* mpMainLogger;
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

	BOOST_REQUIRE(stack.WaitForState(PLS_CLOSED));

	// start up both halfs of the dnp3 connection, vto server port will be online
	stack.masterMgr.Start();
	stack.slaveMgr.Start();
	// startup the loopback server on the slave side
	stack.loopback.Start();

	// startup the local connection to the vto socket, since the whole stack is up we should connect
	stack.local.Start();
	BOOST_REQUIRE(stack.WaitForState(PLS_OPEN));

	// test that data is correctly sent both ways
	HexSequence testData1("de ad be ef");
	stack.local.WriteData(testData1);
	BOOST_REQUIRE(stack.WaitForData(testData1));

	// stop the remote loopback server, which should cause the local vto socket to close and reopen
	stack.loopback.Stop();
	BOOST_REQUIRE(stack.WaitForState(PLS_CLOSED));

	// check that we can reconnect to the loopback if it is turned back on
	stack.loopback.Start();
	BOOST_REQUIRE(stack.WaitForState(PLS_OPEN));

	// send a second set of data
	HexSequence testData2("01 02 03 04");
	stack.local.WriteData(testData2);
	BOOST_REQUIRE(stack.WaitForData(testData2));

	// disconnect again
	stack.loopback.Stop();
	BOOST_REQUIRE(stack.WaitForState(PLS_CLOSED));
}

BOOST_AUTO_TEST_CASE(ServerNotOpenUntilDnpConnected)
{
	VtoTestStack stack;

	BOOST_REQUIRE(stack.WaitForState(PLS_CLOSED));

	// start master side, since there is no dnp connection to slave we wont start listening socket
	stack.masterMgr.Start();

	// start local connection, we wont be able to connect to vto socket because there is no dnp3 connection made yet so we should
	// end up in the waiting state
	stack.local.Start();
	BOOST_REQUIRE(stack.WaitForState(PLS_WAITING));

	// start the slave side of the dnp3 connection and verify that we can connect to the vto server
	stack.slaveMgr.Start();
	BOOST_REQUIRE(stack.WaitForState(PLS_OPEN));

	// since the remote side can't connect to the port we should have our local connection bounced
	BOOST_REQUIRE(stack.WaitForState(PLS_CLOSED));
}

BOOST_AUTO_TEST_CASE(SocketIsClosedIfDnpDrops)
{
	VtoTestStack stack;

	BOOST_REQUIRE(stack.WaitForState(PLS_CLOSED));

	// start all 4 components, should connect
	stack.masterMgr.Start();
	stack.slaveMgr.Start();
	stack.loopback.Start();
	stack.local.Start();

	BOOST_REQUIRE(stack.WaitForState(PLS_OPEN));

	// kill slave dnp3, should kill our local connection
	stack.slaveMgr.Stop();

	BOOST_REQUIRE(stack.WaitForState(PLS_CLOSED));
}

bool False()
{
	return false;
}

BOOST_AUTO_TEST_CASE(SocketIsClosedIfRemoteDrops)
{
	VtoTestStack stack;

	BOOST_REQUIRE(stack.WaitForState(PLS_CLOSED));

	// start all 4 components, should connect
	stack.masterMgr.Start();
	stack.slaveMgr.Start();
	stack.loopback.Start();
	stack.local.Start();

	BOOST_REQUIRE(stack.WaitForState(PLS_OPEN));

	// kill remote connection, should kill our local connection
	stack.mpMainLogger->Log(LEV_EVENT, LOCATION, "Stopping loopback");
	stack.loopback.Stop();
	stack.mpMainLogger->Log(LEV_EVENT, LOCATION, "Stopped loopback");

	//stack.testObj.ProceedUntil(&False, 100000);

	BOOST_REQUIRE(stack.WaitForState(PLS_CLOSED));
}



BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
