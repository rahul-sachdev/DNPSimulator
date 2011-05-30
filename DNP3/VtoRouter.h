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

#include <queue>

#include <APL/IHandlerAsync.h>
#include <APL/AsyncPhysLayerMonitor.h>
#include <APL/ShiftableBuffer.h>

#include "VtoDataInterface.h"

namespace apl {

	class IPhysicalLayerAsync;
	class ITimerSource;

	namespace dnp {

		class VtoReader;
		class VtoWriter;

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
		class VtoRouter : public AsyncPhysLayerMonitor, IVtoCallbacks
		{
			public:

				/**
				 * Create a new VtoRouter instance.  Prior to using this
				 * instance, make sure to register both a IPhysicalLayerAsync
				 * and a VtoWriter using SetPhysicalLayer() and
				 * SetVtoWriter(), respectively.
				 *
				 * @param apLogger			the Logger that the instance
				 * 							should use for log messages
				 * @param aChannelId		the DNP3 Virtual Terminal port
				 * 							(channel id)
				 *
				 * @return					a new VtoRouter instance
				 */
				VtoRouter(Logger* apLogger, boost::uint8_t aChannelId, IVtoWriter* apWriter, IPhysicalLayerAsync* apPhysLayer, ITimerSource *apTimerSrc);

				/**
				 * Receives data from the VTO channel and forwards it to the
				 * IPhysicalLayerAsync instance associated with this VtoRouter.
				 * Implements IVtoCallbacks::OnVtoDataReceived().
				 *
				 * @param apData		The data received from the VTO stream
				 * @param aLength		The length of the data received (in
				 *						bytes)
				 */
				void OnVtoDataReceived(const boost::uint8_t* apData,
				                            size_t aLength);

				/**
				 * Called when the VTO data buffer size changes (startup and
				 * successuly transmission).
				 *
				 * @param aSize			Available space (bytes) in the buffer
				 */
				void OnBufferAvailable(size_t aSize);

				protected:

				void CheckForRead();

				// Implement AsyncPhysLayerMonitor

				void OnPhysicalLayerOpen();

				void OnPhysicalLayerClose();

				/**
				 * Receives data from the physical layer and forwards it to the
				 * VtoWriter.
				 *
				 * @param apData		The data received from the physical
				 * 						layer
				 * @param aLength		The length of the data received (in
				 *						bytes)
				 */
				void _OnReceive(const boost::uint8_t* apData, size_t aLength);

				/**
				 * Implements IUpperLayer::_OnSendSuccess(), which was
				 * inherited via IHandlerAsync.  Called when an asynchronous
				 * transmission to the physical layer was successful.
				 */
				void _OnSendSuccess();

				/**
				 * Implements IUpperLayer::_OnSendFailure(), which was
				 * inherited via IHandlerAsync.  Called when an asynchronous
				 * transmission to the physical layer was not successful.
				 */
				void _OnSendFailure();

			private:

				/**
				 * The VtoWriter instance that will be used to send the data
				 * that is received by the IPhysicalLayerAsync instance to the
				 * VTO endpoint.
				 */
				IVtoWriter* mpVtoWriter;

				/**
				 * The transmit buffer for the physical layer.  The data that
				 * is put into this buffer was originally received via VTO.
				 */
				std::queue<VtoData> mPhysLayerTxBuffer;


				ShiftableBuffer mBuffer;
		};

	}
}

/* vim: set ts=4 sw=4: */

#endif

