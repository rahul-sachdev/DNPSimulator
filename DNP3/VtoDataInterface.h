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

#ifndef __VTO_DATA_INTERFACE_H_
#define __VTO_DATA_INTERFACE_H_

#include <assert.h>

#include "APL/DataTypes.h"
#include "DNP3/ObjectInterfaces.h"

namespace apl {
	namespace dnp {

		/**
		 * The abstract base class for all VTO interface handlers.
		 */
		class IVtoChannel
		{
			public:

				/**
				 * Creates a new IVtoBase instance configured for Virtual
				 * Terminal channel id matching aChannelId.
				 *
				 * @param aChannelId	the DNP3 Virtual Terminal port (channel
				 *						id)
				 *
				 * @return				the new IVtoBase instance
				 */
				IVtoChannel(boost::uint8_t aChannelId) : mChannelId(aChannelId) {}

				/**
				 * Returns the Virtual Terminal channel id for the object.
				 *
				 * @return the DNP3 Virtual Terminal port (channel id)
				 */
				boost::uint8_t GetChannelId()
				{
					return this->mChannelId;
				}

			private:

				/**
				 * A hidden default constructor.
				 *
				 * @deprecated			Use IVtoChannel(boost::uint8_t) instead.
				 */
				IVtoChannel() {}

				/**
				 * The DNP3 Virtual Terminal port (channel id) for this object.
				 */
				boost::uint8_t mChannelId;
		};

		/**
		 * IVTOWriter is returned by the stack for write operations to a Vto
		 * stream.  The Write() function should be used in conjunction with the
		 * OnBufferAvailable() callback on the IVTOCallbacks interface provided
		 * to the stack.
		 */
		class IVtoWriter
		{
			public:
				
				/**
				 * Writes a stream of data to the remote VTO endpoint.
				 *
				 * @param apData		The data to write to the VTO stream.
				 * @param aLength		The length of the data to write (in
				 *						bytes).
				 * @param aChannelId	The channel id for the vto stream
				 *
				 * @return				The number of bytes that were
				 *                      successfully queued into the VTO
				 *                      transmission queue.  This number may be
				 *                      less than the length request if the
				 *                      buffer has insufficient space.
				 */
				virtual size_t Write(const boost::uint8_t* apData, size_t aLength, boost::uint8_t aChannelId) = 0;
		};
		
		/**
		 * Receives data from the stack for a particular channel and is notified
		 * when buffer space becomes available.  Applications that wish to use
		 * the AsyncStackManager::AddVtoChannel() hook must implement a concrete
		 * subclass of this class and register an instance of that subclass
		 * during the function call.
		 */
		class IVtoCallbacks : public IVtoChannel
		{
			public:

				/**
				 * Creates a new IVtoCallbacks instance configured for Virtual
				 * Terminal channel id matching aChannelId.
				 *
				 * @param aChannelId	the DNP3 Virtual Terminal port (channel
				 *						id)
				 *
				 * @return				the new IVtoCallbacks instance
				 */
				IVtoCallbacks(boost::uint8_t aChannelId) : IVtoChannel(aChannelId) {}

				/**
				 * Called when data arrives from stack and needs to be handled.
				 *
				 * @param arData		The data received from the VTO stream.
				 * @param aLength		The length of the data received (in
				 *						bytes).
				 */
				virtual void OnDataReceived(const boost::uint8_t& arData,
				                            size_t aLength) = 0;

				/**
				 * Called when the Vto data buffer size changes (startup and
				 * successuly transmission).
				 *
				 * @param aSize			Available space (bytes) in the buffer
				 */
				virtual void OnBufferAvailable(size_t aSize) = 0;
		};

	}
}

#endif

