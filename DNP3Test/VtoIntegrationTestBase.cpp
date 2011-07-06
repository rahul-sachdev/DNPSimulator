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
#include "VtoIntegrationTestBase.h"

#include <APL/LogToStdio.h>

#include <DNP3/SlaveStackConfig.h>
#include <DNP3/MasterStackConfig.h>
#include <DNP3/VtoRouterSettings.h>

/** Platforms have different reserved port ranges */

namespace apl
{
namespace dnp
{


VtoIntegrationTestBase::VtoIntegrationTestBase(
    bool clientOnSlave,
    bool aImmediateOutput,
    FilterLevel level,
    boost::uint16_t port) :

	LogTester(),
	mpMainLogger(mLog.GetLogger(level, "main")),
	ltf(&mLog, "integration.log", true),
	testObj(),
	manager(mLog.GetLogger(level, "manager")),
	timerSource(testObj.GetService()),
	client(mLog.GetLogger(level, "local-tcp-client"), testObj.GetService(), "127.0.0.1", port + 20),
	server(mLog.GetLogger(level, "loopback-tcp-server"), testObj.GetService(), "0.0.0.0", port + 10)
{

	if(aImmediateOutput) mLog.AddLogSubscriber(LogToStdio::Inst());

	manager.AddTCPServer("dnp-tcp-server", PhysLayerSettings(), "127.0.0.1", port);
	manager.AddSlave("dnp-tcp-server", "slave", level, &cmdAcceptor, SlaveStackConfig());

	manager.AddTCPClient("dnp-tcp-client", PhysLayerSettings(), "127.0.0.1", port);
	manager.AddMaster("dnp-tcp-client", "master", level, &fdo, MasterStackConfig());

	// switch if master or slave gets the loopback half of the server

	std::string clientSideOfStack = clientOnSlave ? "slave" : "master";
	std::string serverSideOfStack = clientOnSlave ? "master" : "slave";

	manager.AddTCPClient("vto-tcp-client", PhysLayerSettings(), "127.0.0.1", port + 10);
	manager.StartVtoRouter("vto-tcp-client", clientSideOfStack, VtoRouterSettings(88, false, false, 1000));
	manager.AddTCPServer("vto-tcp-server", PhysLayerSettings(), "127.0.0.1", port + 20);
	manager.StartVtoRouter("vto-tcp-server", serverSideOfStack, VtoRouterSettings(88, true, false, 1000));
}

VtoIntegrationTestBase::~VtoIntegrationTestBase()
{

}

}

}

/* vim: set ts=4 sw=4: */
