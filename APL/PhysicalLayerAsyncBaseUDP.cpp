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
#include "PhysicalLayerAsyncBaseUDP.h"

#include <boost/bind.hpp>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>

#include "Exception.h"
#include "IHandlerAsync.h"
#include "Logger.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using namespace std;

namespace apl {

PhysicalLayerAsyncBaseUDP::PhysicalLayerAsyncBaseUDP(Logger* apLogger, boost::asio::io_service* apIOService) :
PhysicalLayerAsyncASIO(apLogger, apIOService),
mSocket(*apIOService)
{
}


PhysicalLayerAsyncBaseUDP::~PhysicalLayerAsyncBaseUDP(void)
{
}

void PhysicalLayerAsyncBaseUDP::DoClose()
{
	boost::system::error_code ec;
	mSocket.close(ec);
	if(ec) LOG_BLOCK(LEV_WARNING, ec.message());
}

void PhysicalLayerAsyncBaseUDP::DoOpenSuccess()
{
	//mSocket.set_option(ip::tcp::no_delay(true));
	LOG_BLOCK(LEV_INFO, "Successful conneciton");
}

void PhysicalLayerAsyncBaseUDP::DoOpenFailure()
{
	LOG_BLOCK(LEV_INFO, "Failed socket open, reclosing");
	DoClose();
}

} // apl
