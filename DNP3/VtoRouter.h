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

#ifndef __VTO_ROUTER_H_
#define __VTO_ROUTER_H_

#include <APL/IHandlerAsync.h>
#include <APL/IPhysicalLayerAsync.h>
#include <APL/Logger.h>

#include "VtoReader.h"
#include "VtoWriter.h"

namespace apl {
	namespace dnp {

		/**
		 * Class used to route data between a VTO channel (made up of both a
		 * VtoWriter and VtoReader instance) and an IPhysicalLayerAsync
		 * instance.
		 *
		 * @code
		 *                                           +--> [VtoReader]
		 *                                           |
		 * [IPhysicalLayerAsync] <--> [VtoRouter] <--+
		 *                                           |
		 *                                           +--> [VtoWriter]
		 * @endcode
		 *
		 * The VtoRouter instance provides the necessary IVtoCallbacks hooks
		 * that the VtoReader will use.
		 */
		class VtoRouter : public IVtoCallbacks, public IHandlerAsync
		{
			public:

				/**
				 * Create a new VtoRouter instance.
				 *
				 * @param apLogger			the Logger that the instance
				 * 							should use for log messages
				 * @param aChannelId		the DNP3 Virtual Terminal port
				 * 							(channel id)
				 * @param apVtoWriter		the VtoWriter instance that should
				 * 							be used to write data to the VTO
				 * 							stream
				 * @param apPhysLayer		the IPhysicalLayerAsync instance
				 * 							that is the non-VTO end of the
				 * 							VtoRouter setup
				 *
				 * @return					a new VtoRouter instance
				 */
				VtoRouter(Logger* apLogger, boost::uint8_t aChannelId, VtoWriter* apVtoWriter, IPhysicalLayerAsync* apPhysLayer);

				/**
				 * Receives data from the VTO channel and forwards it to the
				 * IPhysicalLayerAsync instance associated with this VtoRouter.
				 * Implements IVtoCallbacks::OnDataReceived().
				 *
				 * @param apData		The data received from the VTO stream.
				 * @param aLength		The length of the data received (in
				 *						bytes).
				 */
				void OnDataReceived(const boost::uint8_t* apData,
				                            size_t aLength);

				/**
				 * Called when the Vto data buffer size changes (startup and
				 * successuly transmission).  This function is not used in the
				 * VtoRouter implementation.
				 *
				 * @param aSize			Available space (bytes) in the buffer
				 */
				void OnBufferAvailable(size_t aSize) {}

				void OnReceive(const boost::uint8_t* apData, size_t aLength);

			protected:

				void _OnReceive(const boost::uint8_t*, size_t);

				void _OnSendSuccess();

				void _OnSendFailure();

			private:

				void _OnOpenFailure();

				/**
				 * The VtoWriter instance that will be used to send the data
				 * that is received by the IPhysicalLayerAsync instance to the
				 * VTO endpoint.
				 */
				VtoWriter* mpVtoWriter;

				/**
				 * The non-VTO endpoint of this VtoRouter.
				 */
				IPhysicalLayerAsync* mpPhysLayer;

		};

	}
}

/* vim: set ts=4 sw=4: */

#endif

