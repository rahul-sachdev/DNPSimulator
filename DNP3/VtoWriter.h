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
		class VtoWriter : public IVtoWriter, public ITransactable, public SubjectBase<NullLock>
		{
			public:

				/**
				 * Creates a new VtoQueue instance configured for Virtual
				 * Terminal channel id matching aChannelId.
				 *
				 * @param aMaxVtoChunks	Maximum number of 255 byte blocks that
				 * 						can be stored at a time
				 *
				 * @return				the new VtoQueue instance
				 */
				VtoWriter(size_t aMaxVtoChunks);

				/**
				 * Implements IVtoWriter::Write().
				 */
				size_t Write(const boost::uint8_t* apData, size_t aLength, boost::uint8_t aChannelId);

				/**
				 * Returns the number of bytes available to the user
				 * application.  This is a sliding window, so the user
				 * application must manage the data length when calling
				 * VtoWriter::Write().
				 *
				 * @returns				the number of bytes free in the
				 * 						transmission queue
				 */
				size_t NumBytesAvailable();

			protected:

				/**
				 * The ITransactable transaction lock.
				 */
				SigLock mLock;

			private:

				/**
				 * Starts the ITransactable transaction lock.
				 */
				void _Start();

				/**
				 * Ends the ITransactable transaction lock.
				 */
				void _End();

				/**
				 * Returns the number of object chunks available in the
				 * transmission queue.
				 *
				 * @return			the free space available in the queue
				 */
				size_t NumChunksAvailable();

				void Commit(const boost::uint8_t* apData, size_t aLength, boost::uint8_t aChannelId);

				void QueueVtoObject(const boost::uint8_t* apData, size_t aLength, boost::uint8_t aChannelId);

				const size_t mMaxVtoChunks;
				std::queue<VtoEvent> mQueue;
		};
	}
}

/* vim: set ts=4 sw=4: */

#endif
