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

#include "VtoRouter.h"

#include <APL/Exception.h>
#include <APL/IPhysicalLayerAsync.h>
#include <APL/Util.h>

#include "VtoReader.h"
#include "VtoRouterSettings.h"

namespace apl { namespace dnp {
	
VtoRouter::VtoRouter(const VtoRouterSettings& arSettings, Logger* apLogger, IVtoWriter* apWriter, IPhysicalLayerAsync* apPhysLayer, ITimerSource *apTimerSrc) :
	Loggable(apLogger),
	AsyncPhysLayerMonitor(apLogger, apPhysLayer, apTimerSrc, arSettings.OPEN_RETRY_MS),
	IVtoCallbacks(arSettings.CHANNEL_ID),
	mpVtoWriter(apWriter),
	mVtoTxBuffer(arSettings.VTO_TX_BUFFFER_SIZE_IN_BYTES)
{
	assert(apLogger != NULL);
	assert(apWriter != NULL);
	assert(apPhysLayer != NULL);
	assert(apTimerSrc != NULL);
}

void VtoRouter::OnVtoDataReceived(const boost::uint8_t* apData, size_t aLength)
{
	/*
	 * This will create a container object that allows us to hold the data
	 * pointer asynchronously.  We need to release the object from the queue in
	 * _OnSendSuccess().  Each call is processed serially, so we can take
	 * advantage of the FIFO structure to keep things simple.
	 */
	VtoData vto(apData, aLength);
	this->mPhysLayerTxBuffer.push(vto);
	this->CheckForPhysWrite();	
}

void VtoRouter::_OnReceive(const boost::uint8_t* apData, size_t aLength)
{
	/* Record how much we actual wrote into the buffer */
	mVtoTxBuffer.AdvanceWrite(aLength);

	this->CheckForVtoWrite();
	this->CheckForPhysRead();
}

void VtoRouter::CheckForVtoWrite()
{
	if(mVtoTxBuffer.NumReadBytes() > 0) {
		size_t numWritten = mpVtoWriter->Write(mVtoTxBuffer.ReadBuff(), mVtoTxBuffer.NumReadBytes(), this->GetChannelId());
		mVtoTxBuffer.AdvanceRead(numWritten);
		mVtoTxBuffer.Shift();
		this->CheckForPhysRead();
	}
}

void VtoRouter::_OnSendSuccess()
{	
	/*
	 * If this function has been called, it means that we can now discard the
	 * data that is at the head of the FIFO queue.
	 */
	this->mPhysLayerTxBuffer.pop();
	this->CheckForPhysWrite();
}

void VtoRouter::_OnSendFailure()
{
	/* try to re-transmit the last packet */
	this->CheckForPhysWrite();
}

void VtoRouter::CheckForPhysRead()
{
	if(mpPhys->CanRead()) {
		size_t num = mVtoTxBuffer.NumWriteBytes();
		if(num > 0) {
			mpPhys->AsyncRead(mVtoTxBuffer.WriteBuff(), num);
		}
	}
}

void VtoRouter::CheckForPhysWrite()
{
	if(mpPhys->CanWrite()) {
		if(mPhysLayerTxBuffer.size() > 0) {
			mpPhys->AsyncWrite(mPhysLayerTxBuffer.front().GetData(), mPhysLayerTxBuffer.front().GetSize());
		}
	}
}

void VtoRouter::OnBufferAvailable()
{	
	this->CheckForVtoWrite();
}

void VtoRouter::OnPhysicalLayerOpen()
{
	this->CheckForPhysRead();
	this->CheckForPhysWrite();
}
				
void VtoRouter::OnPhysicalLayerClose()
{

}

}}

/* vim: set ts=4 sw=4: */

