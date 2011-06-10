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

#include <boost/bind.hpp>

#include <APL/Exception.h>
#include <APL/IPhysicalLayerAsync.h>
#include <APL/Util.h>

#include "VtoReader.h"
#include "VtoRouterSettings.h"

namespace apl
{
namespace dnp
{

VtoRouter::VtoRouter(const VtoRouterSettings& arSettings, Logger* apLogger, IVtoWriter* apWriter, IPhysicalLayerAsync* apPhysLayer, ITimerSource* apTimerSrc) :
	Loggable(apLogger),
	AsyncPhysLayerMonitor(apLogger, apPhysLayer, apTimerSrc, arSettings.OPEN_RETRY_MS),
	IVtoCallbacks(arSettings.CHANNEL_ID),
	CleanupHelper(apTimerSrc),
	mpVtoWriter(apWriter),
	mVtoTxBuffer(arSettings.VTO_TX_BUFFFER_SIZE_IN_BYTES),
	mReopenPhysicalLayer(false),
	mPermanentlyStopped(false),
	mCleanedup(false)
{
	assert(apLogger != NULL);
	assert(apWriter != NULL);
	assert(apPhysLayer != NULL);
	assert(apTimerSrc != NULL);

}

void VtoRouter::StopRouter()
{
	mPermanentlyStopped = true;
	mpTimerSrc->Post(boost::bind(&VtoRouter::DoStopRouter, this));
}

void VtoRouter::OnDnpConnectedChanged(bool aConnected)
{
	if(!mPermanentlyStopped) {
		mpTimerSrc->Post(boost::bind(&VtoRouter::DoDnpConnectedChanged, this, aConnected));
	}
}

void VtoRouter::OnVtoRemoteConnectedChanged(bool aConnected)
{
	if(!mPermanentlyStopped) {
		mpTimerSrc->Post(boost::bind(&VtoRouter::DoVtoRemoteConnectedChanged, this, aConnected));
	}
}

void VtoRouter::DoStopRouter()
{
	this->Stop();
}

void VtoRouter::OnVtoDataReceived(const boost::uint8_t* apData, size_t aLength)
{
	LOG_BLOCK(LEV_DEBUG, "GotRemoteData: " << aLength);
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

void VtoRouter::DoStart()
{
	if(mPermanentlyStopped) {
		LOG_BLOCK(LEV_INFO, "Permenantly Stopped")
	}else{
		if(!mReopenPhysicalLayer){
			mReopenPhysicalLayer = true;
			LOG_BLOCK(LEV_INFO, "Starting VtoRouted Port")
			this->Start();
		}
		else {
			LOG_BLOCK(LEV_INFO, "Already started")
		}
	}
}

void VtoRouter::DoStop()
{

	if(mReopenPhysicalLayer){
		mReopenPhysicalLayer = false;
		LOG_BLOCK(LEV_INFO, "Stopping VtoRouted Port")
		this->Stop();
	}
	else {
		LOG_BLOCK(LEV_INFO, "Already stopped")
	}
}

void VtoRouter::_OnReceive(const boost::uint8_t* apData, size_t aLength)
{
	/* Record how much we actual wrote into the buffer */
	mVtoTxBuffer.AdvanceWrite(aLength);

	LOG_BLOCK(LEV_DEBUG, "GotLocalData: " << aLength);

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
			mpPhys->AsyncWrite(mPhysLayerTxBuffer.front().mpData, mPhysLayerTxBuffer.front().GetSize());
		}
	}
}

void VtoRouter::OnBufferAvailable()
{
	this->CheckForVtoWrite();
}

void VtoRouter::OnPhysicalLayerOpen()
{
	LOG_BLOCK(LEV_INFO, "Local Connection Opened");
	this->SetLocalConnected(true);

	this->CheckForPhysRead();
	this->CheckForPhysWrite();
}

void VtoRouter::OnStateChange(PhysLayerState aState)
{
	if(mPermanentlyStopped && aState == PLS_STOPPED && !mCleanedup) {
		mCleanedup = true;
		this->Cleanup();
	}
}

void VtoRouter::OnPhysicalLayerClose()
{
	LOG_BLOCK(LEV_INFO, "Local Connection Closed");
	this->SetLocalConnected(false);
}

void VtoRouter::OnPhysicalLayerOpenFailure()
{
	LOG_BLOCK(LEV_INFO, "Local Connection Open Failed");

	this->SetLocalConnected(false);
}

}
}

/* vim: set ts=4 sw=4: */

