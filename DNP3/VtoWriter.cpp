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

#include "VtoWriter.h"

#include <APL/Util.h>

namespace apl {
	namespace dnp {

		VtoWriter::VtoWriter(size_t aMaxVtoChunks) : 			
			mMaxVtoChunks(aMaxVtoChunks)
		{}
		
		size_t VtoWriter::Write(const boost::uint8_t* apData, size_t aLength, boost::uint8_t aChannelId)
		{
			CriticalSection cs(&mLock); // thread safe section begins
			
			size_t num = Min<size_t>(this->NumBytesAvailable(), aLength);

			this->Commit(apData, num, aChannelId);

			if(num > 0) this->NotifyAll(); // calls notifiers that the queue has data to be read

			return num;
		}

		void VtoWriter::_Start()
		{
			mLock.Lock();
		}
		
		void VtoWriter::_End()
		{						
			mLock.Unlock();			
		}

		void VtoWriter::Commit(const boost::uint8_t* apData, size_t aLength, boost::uint8_t aChannelId)
		{
			size_t complete = aLength / VtoData::MAX_SIZE;
			size_t partial = aLength % VtoData::MAX_SIZE;

			const boost::uint8_t* pLocation = apData;

			for(size_t i=0; i<complete; ++i) {				
				QueueVtoObject(pLocation, VtoData::MAX_SIZE, aChannelId);				
				pLocation += VtoData::MAX_SIZE;
			}

			if(partial > 0) QueueVtoObject(pLocation, partial, aChannelId);				
		}

		void VtoWriter::QueueVtoObject(const boost::uint8_t* apData, size_t aLength, boost::uint8_t aChannelId)
		{
			VtoData vto(apData, aLength);	
			VtoEvent evt(vto, PC_CLASS_1, aChannelId);
			mQueue.push(evt);
		}

		size_t VtoWriter::NumChunksAvailable()
		{
			return mMaxVtoChunks - mQueue.size();
		}

		size_t VtoWriter::NumBytesAvailable()
		{
			return this->NumChunksAvailable()*VtoData::MAX_SIZE;
		}
		
		
}}

