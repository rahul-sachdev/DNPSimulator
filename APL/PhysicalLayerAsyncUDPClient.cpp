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
#include "PhysicalLayerAsyncUDPClient.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <string>

#include "Exception.h"
#include "IHandlerAsync.h"
#include "Logger.h"

using namespace boost;
using namespace boost::asio;
using namespace std;

namespace apl {

PhysicalLayerAsyncUDPClient::PhysicalLayerAsyncUDPClient(
	Logger* apLogger,
	boost::asio::io_service* apIOService,
	const std::string& arAddress,
	boost::uint16_t aPort,
	bool aBroadcast) :
	
	PhysicalLayerAsyncBaseUDP(apLogger, apIOService),
	mAddr(arAddress),
	mPort(aPort)
{
	mBroadcast = aBroadcast;
}


PhysicalLayerAsyncUDPClient::~PhysicalLayerAsyncUDPClient(void)
{
}

/* Implement the actions */
void PhysicalLayerAsyncUDPClient::DoOpen()
{
	ip::address_v4 address;
	boost::system::error_code ec;
	string ipstring(mAddr);
	ip::udp::endpoint ep;

	address = address.from_string(ipstring, ec);

	if(ec) throw ArgumentException(LOCATION, "string Address: " + ipstring + " is invalid");
	
	// ip::udp::endpoint serverEndpoint(address, mPort);
	mRemoteEndpoint.address(address);
	mRemoteEndpoint.port(mPort);

	mSocket.open(mRemoteEndpoint.protocol(), ec);

	// bind to a random local port
	mSocket.bind(ep, ec);

	if (mBroadcast)
	{
		boost::asio::socket_base::broadcast option(true);

		mSocket.set_option(option, ec);
		if(ec) throw Exception(LOCATION, ec.message());
	}

	// since there's no connect in a UDP connection, trigger the callback
	// that the connect is complete
	// this->OnOpenCallback(ec);
	mpService->post(boost::bind(&PhysicalLayerAsyncUDPClient::OnOpenCallback, this, ec));
	// mpService->post(boost::bind(&PhysicalLayerAsyncSerial::OnOpenCallback, this, ec));

}

void PhysicalLayerAsyncUDPClient::DoAsyncWrite(const boost::uint8_t* apBuffer, size_t aNumBytes)
{
	mSocket.async_send_to(buffer(apBuffer, aNumBytes), mRemoteEndpoint, boost::bind(&PhysicalLayerAsyncUDPClient::OnWriteCallback, this, boost::asio::placeholders::error, aNumBytes));
}

void PhysicalLayerAsyncUDPClient::DoAsyncRead(boost::uint8_t* apBuffer, size_t aMaxBytes)
{
	mSocket.async_receive_from(buffer(apBuffer, aMaxBytes), mRemoteEndpoint,
		boost::bind(&PhysicalLayerAsyncUDPClient::OnReadCallback, this, boost::asio::placeholders::error, apBuffer, boost::asio::placeholders::bytes_transferred));

//	mSocket.async_receive(buffer(apBuffer, aMaxBytes),
//		boost::bind(&PhysicalLayerAsyncUDPClient::OnReadCallback, this, boost::asio::placeholders::error, apBuffer, boost::asio::placeholders::bytes_transferred));
}

}
