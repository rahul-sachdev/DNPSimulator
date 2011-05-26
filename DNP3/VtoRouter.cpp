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

#include <APL/Exception.h>

#include "VtoRouter.h"

using apl::CriticalSection;
using apl::IPhysicalLayerAsync;
using apl::Logger;
using apl::NotImplementedException;
using apl::dnp::VtoRouter;

VtoRouter::VtoRouter(Logger* apLogger, boost::uint8_t aChannelId) :
	Loggable(apLogger),
	IHandlerAsync(apLogger),
	IVtoCallbacks(aChannelId),
	mpVtoWriter(NULL),
	mpPhysLayer(NULL)
{}

void VtoRouter::SetPhysicalLayer(IPhysicalLayerAsync* apPhysLayer)
{
	/* The whole function is thread-safe, from start to finish. */
	CriticalSection cs(&mLock);

	assert(apPhysLayer != NULL);
	assert(this->mpPhysLayer == NULL);

	this->mpPhysLayer = apPhysLayer;
	this->mpPhysLayer->SetHandler(this);
}

void VtoRouter::SetVtoWriter(VtoWriter *apVtoWriter)
{
	/* The whole function is thread-safe, from start to finish. */
	CriticalSection cs(&mLock);

	assert(apVtoWriter != NULL);
	assert(this->mpVtoWriter == NULL);

	this->mpVtoWriter = apVtoWriter;
}

void VtoRouter::OnVtoDataReceived(const boost::uint8_t* apData, size_t aLength)
{
	/* The whole function is thread-safe, from start to finish. */
	CriticalSection cs(&mLock);

	/* Make sure we're not going to write to a null pointer */
	assert(this->mpPhysLayer != NULL);

	/*
	 * This will create a container object that allows us to hold the data
	 * pointer asynchronously.  We need to release the object from the queue in
	 * _OnSendSuccess().  Each call is processed serially, so we can take
	 * advantage of the FIFO structure to keep things simple.
	 */
	VtoData vto(apData, aLength);
	this->mPhysLayerTxBuffer.push(vto);
	this->mpPhysLayer->AsyncWrite(apData, aLength);
}

void VtoRouter::_OnReceive(const boost::uint8_t* apData, size_t aLength)
{
	/*
	 * Pipe the data straight from the physical layer into the VtoWriter.  No
	 * need to make this function thread-safe, since the VtoWriter itself is
	 * already thread-safe.
	 */
	this->mpVtoWriter->Write(apData, aLength, this->GetChannelId());
}

void VtoRouter::_OnSendSuccess()
{
	/* The whole function is thread-safe, from start to finish. */
	CriticalSection cs(&mLock);

	/* Make sure we're not going to write to a null pointer */
	assert(this->mpPhysLayer != NULL);

	/*
	 * If this function has been called, it means that we can now discard the
	 * data that is at the head of the FIFO queue.
	 */
	this->mPhysLayerTxBuffer.pop();
}

void VtoRouter::_OnSendFailure()
{
	/* Just do the same as _OnSendSuccess for now */
	this->_OnSendSuccess();
}

void VtoRouter::_OnOpenFailure()
{
	/* TODO - anything to do? */
	throw NotImplementedException(LOCATION);
}

void VtoRouter::_Start()
{
	this->mLock.Lock();
}

void VtoRouter::_End()
{
	this->mLock.Unlock();
}

/* vim: set ts=4 sw=4: */

