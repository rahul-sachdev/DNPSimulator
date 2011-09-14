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

#include "PhysicalLayerAsyncUDPServer.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <string>

#include "Exception.h"
#include "IHandlerAsync.h"
#include "Logger.h"

using namespace boost;
using namespace boost::system;
using namespace boost::asio;
using namespace std;

namespace apl {

PhysicalLayerAsyncUDPServer::PhysicalLayerAsyncUDPServer(Logger* apLogger, io_service* apIOService, const std::string& arEndpoint, boost::uint16_t aPort, bool aBroadcast) :
PhysicalLayerAsyncBaseUDP(apLogger, apIOService),
mEndpoint(ip::udp::v4(), aPort)
{
	//set the endpoint's address
	boost::system::error_code ec;
	ip::address_v4 addr = ip::address_v4::from_string(arEndpoint, ec);	
	if(ec) throw ArgumentException(LOCATION, "endpoint: " + arEndpoint + " is invalid ");	
	mEndpoint.address(addr);

	mBroadcast = aBroadcast;
}


PhysicalLayerAsyncUDPServer::~PhysicalLayerAsyncUDPServer()
{
}

/* Implement the actions */
void PhysicalLayerAsyncUDPServer::DoOpen()
{
	boost::system::error_code ec;
	
	if(!mSocket.is_open())
	{		
		mSocket.open(mEndpoint.protocol(), ec);
		if(ec) throw Exception(LOCATION, ec.message());

		mSocket.set_option(ip::udp::socket::reuse_address(true));
		mSocket.bind(mEndpoint, ec);
		if(ec) throw Exception(LOCATION, ec.message());

		if (mBroadcast)
		{
			boost::asio::socket_base::broadcast option(true);

			mSocket.set_option(option, ec);
			if(ec) throw Exception(LOCATION, ec.message());
		}

		// this->OnOpenCallback(ec);

		// mAcceptor.listen(socket_base::max_connections, ec);
		// if(ec) throw Exception(LOCATION, ec.message());
	}
	
	mpService->post(boost::bind(&PhysicalLayerAsyncUDPServer::OnOpenCallback, this, ec));
}

void PhysicalLayerAsyncUDPServer::DoOpeningClose()
{
	// mAcceptor.cancel();
	// mSocket.close();
}

void PhysicalLayerAsyncUDPServer::DoAsyncWrite(const boost::uint8_t* apBuffer, size_t aNumBytes)
{
	mSocket.async_send_to(buffer(apBuffer, aNumBytes), mRemoteEndpoint, boost::bind(&PhysicalLayerAsyncUDPServer::OnWriteCallback, this, boost::asio::placeholders::error, aNumBytes));
}

void PhysicalLayerAsyncUDPServer::DoAsyncRead(boost::uint8_t* apBuffer, size_t aMaxBytes)
{
	mSocket.async_receive_from(buffer(apBuffer, aMaxBytes), mRemoteEndpoint,
		boost::bind(&PhysicalLayerAsyncUDPServer::OnReadCallback, this, boost::asio::placeholders::error, apBuffer, boost::asio::placeholders::bytes_transferred));
}

}
