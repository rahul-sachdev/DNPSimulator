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

#ifndef __VTO_QUEUE_H_
#define __VTO_QUEUE_H_

#include <queue>

#include <APL/Lock.h>
#include <APL/DataInterfaces.h>
#include <APL/SubjectBase.h>

#include "EventTypes.h"
#include "VtoDataInterface.h"

namespace apl {
	namespace dnp {

		/**
		 * Implements the IVTOWriter interface that is handed out by the stack
		 * Responsible for UserCode -> Stack thread marshalling and stream decomposition.
		 */
		class VtoQueue : public IVtoWriter, public ITransactable, public SubjectBase<NullLock>
		{
			public:

				/**
				 * Creates a new VtoQueue instance configured for Virtual
				 * Terminal channel id matching aChannelId.
				 *
				 * @param aMaxVtoChunks	Maximum number of 255 byte blocks that can be stored at a time
				 *
				 * @return				the new VtoQueue instance
				 */
				VtoQueue(size_t aMaxVtoChunks = 10);
				
				/// Implements IVtoWriter::Write
				size_t Write(const boost::uint8_t* apData, size_t aLength, boost::uint8_t aChannelId);

				size_t NumBytesAvailable();
				
			private:

				// concrete classes will define these functions
				void _Start();
				void _End();
				
				size_t NumChunksAvailable();
				
				void Commit(const boost::uint8_t* apData, size_t aLength, boost::uint8_t aChannelId);
				
				void QueueVtoObject(const boost::uint8_t* apData, size_t aLength, boost::uint8_t aChannelId);

				SigLock mLock;
				const size_t mMaxVtoChunks;
				std::queue<VtoEvent> mQueue;				
		};

}}

#endif
