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

#ifndef __VTO_WRITER_H_
#define __VTO_WRITER_H_

#include <queue>
#include <set>

#include <APL/Lock.h>
#include <APL/DataInterfaces.h>
#include <APL/SubjectBase.h>

#include "EventTypes.h"
#include "VtoDataInterface.h"
#include "VtoData.h"

namespace apl {
	namespace dnp {
		
		/**
		 * Implements the IVTOWriter interface that is handed out by the
		 * stack.  Responsible for UserCode -> Stack thread marshalling and
		 * stream decomposition.
		 */
		class VtoWriter : public IVtoWriter, public SubjectBase<NullLock>
		{
			public:

				/**
				 * Creates a new VtoQueue instance.  The aMaxVtoChunks
				 * parameter defines the maximum number of 255-byte blocks
				 * that can be buffered at a time.
				 *
				 * @param aMaxVtoChunks	Maximum number of 255-byte blocks that
				 *                      can be stored at a time
				 *
				 * @return				the new VtoQueue instance
				 */
				VtoWriter(size_t aMaxVtoChunks);
				
				/**
				* Registers an IVtoCallbacks to receive OnBufferAvailable() notifications
				* @param apCallbacks The interface to invoke when space is made available
				*/
				void AddVtoCallback(IVtoCallbacks* apCallbacks);

				/**
				* Stops an IVtoCallbacks from receiving OnBufferAvailable() notifications
				* @param apCallbacks The interface to stop calling when space is available
				*/
				void RemoveVtoCallback(IVtoCallbacks* apCallbacks);

				/**
				 * Implements IVtoWriter::Write().
				 */
				size_t Write(const boost::uint8_t* apData,
				             size_t aLength,
				             boost::uint8_t aChannelId);

				/**
				 * Reads one item from the front of the queue.  If no items
				 * are available, the function returns false.  If an item is
				 * found in the queue, the item is stored in arEvent and
				 * removed from the queue, and the function returns true.
				 *
				 * @param arEvent		The destination store for the queue
				 * 						data
				 *
				 * @return				true if data is returned, false
				 * 						otherwise
				 */
				bool Read(VtoEvent& arEvent);

				/**
				 * Implements IVtoWriter::Size().
				 */
				size_t Size();

				/**
				 * Returns the number of bytes available to the user
				 * application.  This is a sliding window, so the user
				 * application must manage the data length when calling
				 * VtoWriter::Write().
				 *
				 * @returns				the number of bytes free in the
				 *                      transmission queue
				 */
				size_t NumBytesAvailable();

			protected:

				/**
				 * Lock used for thread safety
				 */
				SigLock mLock;

			private:

				bool ReadWithoutNotifying(VtoEvent& arEvent);

				void NotifyAllCallbacks();
				
				/**
				 * Returns the number of object chunks available in the
				 * transmission queue.
				 *
				 * @return			the free space available in the queue
				 */
				size_t NumChunksAvailable();

				void Commit(const boost::uint8_t* apData,
				            size_t aLength,
				            boost::uint8_t aChannelId);

				void QueueVtoObject(const boost::uint8_t* apData,
				                    size_t aLength,
				                    boost::uint8_t aChannelId);

				const size_t mMaxVtoChunks;
				std::queue<VtoEvent> mQueue;

				typedef std::set<IVtoCallbacks*> CallbackSet;
				CallbackSet mCallbacks;
		};
	}
}

/* vim: set ts=4 sw=4: */

#endif

