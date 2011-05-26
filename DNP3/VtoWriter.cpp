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

using apl::CriticalSection;
using apl::dnp::VtoData;
using apl::dnp::VtoEvent;
using apl::dnp::VtoWriter;

VtoWriter::VtoWriter(size_t aMaxVtoChunks) :
	mMaxVtoChunks(aMaxVtoChunks)
{}

size_t VtoWriter::Write(const boost::uint8_t* apData,
                        size_t aLength,
                        boost::uint8_t aChannelId)
{
	/*
	 * The whole function is thread-safe, from start to finish.
	 */
	CriticalSection cs(&mLock);

	/*
	 * Only write the maximum amount available or requested.  If the
	 * requested data size is larger than the available buffer space,
	 * only send what will fit.
	 */
	size_t num = Min<size_t>(this->NumBytesAvailable(), aLength);

	/*
	 * Chop up the data into Max(255) segments and add it to the queue.
	 */
	this->Commit(apData, num, aChannelId);

	/* Tell any listeners that the queue has new data to be read. */
	if (num > 0)
		this->NotifyAll();

	/* Return the number of bytes from apData that were queued. */
	return num;
}

void VtoWriter::_Start()
{
	this->mLock.Lock();
}

void VtoWriter::_End()
{
	this->mLock.Unlock();
}

void VtoWriter::Commit(const boost::uint8_t* apData,
                       size_t aLength,
                       boost::uint8_t aChannelId)
{
	/*
	 * The data is segmented into N number of chunks, each of MAX_SIZE
	 * bytes, and 1 chunk containing the remainder of the data that is
	 * less than MAX_SIZE bytes.  We pre-calculate the chunk size to
	 * avoid the constant comparison overhead in the loop itself.
	 */
	size_t complete = aLength / VtoData::MAX_SIZE;
	size_t partial = aLength % VtoData::MAX_SIZE;

	/* First, write the full-sized blocks */
	for (size_t i = 0; i < complete; ++i)
	{
		this->QueueVtoObject(apData, VtoData::MAX_SIZE, aChannelId);
		apData += VtoData::MAX_SIZE;
	}

	/* Next, write the remaining data at the end of the stream */
	if (partial > 0)
		this->QueueVtoObject(apData, partial, aChannelId);
}

void VtoWriter::QueueVtoObject(const boost::uint8_t* apData,
                               size_t aLength,
                               boost::uint8_t aChannelId)
{
	/*
	 * Create a new VtoData instance, set the event data associated
	 * with it, and then push the object onto the transmission queue.
	 */
	VtoData vto(apData, aLength);

	VtoEvent evt(vto, PC_CLASS_1, aChannelId);
	this->mQueue.push(evt);
}

bool VtoWriter::Read(VtoEvent& arEvent)
{
	/*
	 * The whole function is thread-safe, from start to finish.
	 */
	CriticalSection cs(&mLock);

	/*
	 * If there is no data on the queue, return false.  This allows
	 * the function to be used in the conditional block of a loop
	 * construct.
	 */
	if (this->mQueue.size() == 0)
	{
		return false;
	}

	/*
	 * The queue has data, so pop off the front item, store it in
	 * arEvent, and return true.
	 */
	arEvent = this->mQueue.front();
	this->mQueue.pop();
	return true;
}

size_t VtoWriter::Size()
{
	/*
	 * The whole function is thread-safe, from start to finish.
	 */
	CriticalSection cs(&mLock);

	return mQueue.size();
}

size_t VtoWriter::NumChunksAvailable()
{
	return mMaxVtoChunks - mQueue.size();
}

size_t VtoWriter::NumBytesAvailable()
{
	return this->NumChunksAvailable() * VtoData::MAX_SIZE;
}

/* vim: set ts=4 sw=4: */

