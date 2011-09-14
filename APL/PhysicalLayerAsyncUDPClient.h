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

#ifndef __PHYSICAL_LAYER_ASYNC_UDP_CLIENT_H_
#define __PHYSICAL_LAYER_ASYNC_UDP_CLIENT_H_

#include "PhysicalLayerAsyncBaseUDP.h"

using namespace boost::asio;

namespace apl {

	class PhysicalLayerAsyncUDPClient : public PhysicalLayerAsyncBaseUDP
	{
		public:
			PhysicalLayerAsyncUDPClient(Logger*, boost::asio::io_service* apIOService, const std::string&, boost::uint16_t aPort, bool aBroadcast = false);

			virtual ~PhysicalLayerAsyncUDPClient();

			/* Implement the remaining actions */
			void DoOpen();

			void DoAsyncRead(boost::uint8_t*, size_t);
			void DoAsyncWrite(const boost::uint8_t*, size_t);

		private:
			std::string mAddr;
			boost::uint16_t mPort;

			bool mBroadcast;

			ip::udp::endpoint mRemoteEndpoint;
	};

}

#endif
