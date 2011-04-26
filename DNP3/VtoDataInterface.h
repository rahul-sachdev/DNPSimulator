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

#include <APL/DataTypes.h>

#include <assert.h>

#include "ObjectInterfaces.h"

namespace apl {
	namespace dnp {

		/**
		 * The abstract base class for all VTO interface handlers.
		 */
		class IVtoBase
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
				IVtoBase(byte_t aChannelId) : mChannelId(aChannelId) {}

				/**
				 * Returns the Virtual Terminal channel id for the object.
				 *
				 * @return the DNP3 Virtual Terminal port (channel id)
				 */
				byte_t GetChannelId()
				{
					return this->mChannelId;
				}

			private:

				/**
				 * A hidden default constructor.
				 *
				 * @deprecated			Use IVtoBase(byte_t) instead.
				 */
				IVtoBase() {}

				/**
				 * The DNP3 Virtual Terminal port (channel id) for this object.
				 */
				byte_t mChannelId;
		};

		/**
		 * IVTOWriter is returned by the stack for write operations to a Vto
		 * stream.  The Write() function should be used in conjunction with the
		 * OnBufferAvailable() callback on the IVTOCallbacks interface provided
		 * to the stack.
		 */
		class IVtoWriter : protected IVtoBase
		{
			public:

				/**
				 * Creates a new IVtoWriter instance configured for Virtual
				 * Terminal channel id matching aChannelId.
				 *
				 * @param aChannelId	the DNP3 Virtual Terminal port (channel
				 *						id)
				 *
				 * @return				the new IVtoWriter instance
				 */
				IVtoWriter(byte_t aChannelId) : IVtoBase(aChannelId) {}

				/**
				 * Writes a stream of data to the remote VTO endpoint.
				 *
				 * @param arData		The data to write to the VTO stream.
				 * @param aLength		The length of the data to write (in
				 *						bytes).
				 *
				 * @return				The number of bytes that were
				 *                      successfully queued into the VTO
				 *                      transmission queue.  This number may be
				 *                      less than the length request if the
				 *                      buffer has insufficient space.
				 */
				size_t Write(const byte_t& arData, size_t aLength);

			protected:

				/**
				 * Returns the appropriate DNP3 Object Group instance for the
				 * IVtoWriter instance.  For example, VtoMasterWriter should
				 * most likely return Group112Var0; VtoSlaveWriter should most
				 * likely return Group113Var0.
				 *
				 * @returns				the appropriate ObjectBase instance
				 *						needed during the Write() operation.
				 */
				virtual SizeByVariationObject* GetObjectGroupInstance() = 0;

		};

		/**
		 * Implements the writer interface needed for a Master implementing the
		 * DNP3 VTO feature.
		 */
		class VtoMasterWriter : protected IVtoWriter
		{
			public:

				/**
				 * Creates a new VtoMasterWriter instance configured for Virtual
				 * Terminal channel id matching aChannelId.
				 *
				 * @param aChannelId	the DNP3 Virtual Terminal port (channel
				 *						id)
				 *
				 * @return				the new VtoMasterWriter instance
				 */
				VtoMasterWriter(byte_t aChannelId) : IVtoWriter(aChannelId) {}

			protected:

				/**
				 * Returns a Group112Var0 object instance.
				 */
				SizeByVariationObject* GetObjectGroupInstance();
		};

		/**
		 * Implements the writer interface needed for a Slave implementing the
		 * DNP3 VTO feature.
		 */
		class VtoSlaveWriter : protected IVtoWriter
		{
			public:

				/**
				 * Creates a new VtoSlaveWriter instance configured for Virtual
				 * Terminal channel id matching aChannelId.
				 *
				 * @param aChannelId	the DNP3 Virtual Terminal port (channel
				 *						id)
				 *
				 * @return				the new VtoSlaveWriter instance
				 */
				VtoSlaveWriter(byte_t aChannelId) : IVtoWriter(aChannelId) {}

			protected:

				/**
				 * Returns a Group113Var0 object instance.
				 */
				SizeByVariationObject* GetObjectGroupInstance();
		};

		/**
		 * Receives data from the stack for a particular channel and is notified
		 * when buffer space becomes available.  Applications that wish to use
		 * the AsyncStackManager::AddVtoChannel() hook must implement a concrete
		 * subclass of this class and register an instance of that subclass
		 * during the function call.
		 */
		class IVtoCallbacks : protected IVtoBase
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
				IVtoCallbacks(byte_t aChannelId) : IVtoBase(aChannelId) {}

				/**
				 * Called when data arrives from stack and needs to be handled.
				 *
				 * @param arData		The data received from the VTO stream.
				 * @param aLength		The length of the data received (in
				 *						bytes).
				 */
				virtual void OnDataReceived(const byte_t& arData,
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

