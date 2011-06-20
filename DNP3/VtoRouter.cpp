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
	mReopenPhysicalLayer(false),
	mPermanentlyStopped(false),
	mCleanedup(false)
{
	assert(apLogger != NULL);
	assert(apWriter != NULL);
	assert(apPhysLayer != NULL);
	assert(apTimerSrc != NULL);

}

VtoRouter::~VtoRouter(){
	size_t s = mVtoTxBuffer.size();
	while(s-- >0) {
		delete mVtoTxBuffer.front();
		mVtoTxBuffer.pop();
	}
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

void VtoRouter::DoStopRouter()
{
	this->Stop();
}

void VtoRouter::OnVtoDataReceived(const VtoData& arData)
{
	LOG_BLOCK(LEV_DEBUG, "GotRemoteData: " << arData.GetSize() << " Type: " << arData.GetType());

	/*
	 * This will create a container object that allows us to hold the data
	 * pointer asynchronously.  We need to release the object from the queue in
	 * _OnSendSuccess().  Each call is processed serially, so we can take
	 * advantage of the FIFO structure to keep things simple.
	 */
	this->mPhysLayerTxBuffer.push(arData);
	this->CheckForPhysWrite();
}

void VtoRouter::DoStart()
{
	if(mPermanentlyStopped) {
		LOG_BLOCK(LEV_INFO, "Permenantly Stopped")
	}
	else {
		if(!mReopenPhysicalLayer) {
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

	if(mReopenPhysicalLayer) {
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
	LOG_BLOCK(LEV_DEBUG, "GotLocalData: " << aLength);
	mVtoTxBuffer.back()->mBuffer.AdvanceWrite(aLength);

	this->CheckForVtoWrite();
	this->CheckForPhysRead();
}

void VtoRouter::CheckForVtoWrite()
{
	// need to check mPermanentlyStopped, we will often get a physical layer closed notification after
	// we have already stopped and disposed of the dnp3 stack so we need to not call anything on mpVtoWriter
	if(!mPermanentlyStopped && !mVtoTxBuffer.empty()){
		VtoDataType type = mVtoTxBuffer.front()->mType;
		// type DATA means this is a buffer and we need to pull the data out and send it to the vto writer
		if(type == DATA){
			ShiftableBuffer* pBuffer = &mVtoTxBuffer.front()->mBuffer;
			size_t num = pBuffer->NumReadBytes();
			if(num > 0) {
				size_t numWritten = mpVtoWriter->Write(pBuffer->ReadBuff(), num, this->GetChannelId());
				LOG_BLOCK(LEV_DEBUG, "VtoWriter: " << numWritten << " of " << num);
				pBuffer->AdvanceRead(numWritten);
				pBuffer->Shift();
				this->CheckForPhysRead();
			}
			// if the buffer was empty and we have other types of data to send pop the buffer off and recheck
			if(num == 0 && mVtoTxBuffer.size() > 1){
				delete mVtoTxBuffer.front();
				mVtoTxBuffer.pop();
				this->CheckForVtoWrite();
			}
		}else{
			// if we have generated REMOTE_OPENED or REMOTE_CLOSED message we need to send the SetLocalVtoState
			// update to the vtowriter so it can be serialized in the correct order. 
			mpVtoWriter->SetLocalVtoState(type == REMOTE_OPENED, this->GetChannelId());
			delete mVtoTxBuffer.front();
			mVtoTxBuffer.pop();
			this->CheckForVtoWrite();
		}
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
		// the last entry in the buffer will always be the DATA buffer if we have a local 
		// connection open.
		ShiftableBuffer* pBuffer = &mVtoTxBuffer.back()->mBuffer;
		size_t num = pBuffer->NumWriteBytes();
		if(num > 0) {
			mpPhys->AsyncRead(pBuffer->WriteBuff(), num);
		}
	}
}

void VtoRouter::CheckForPhysWrite()
{
	if(!mPhysLayerTxBuffer.empty()) {
		VtoDataType type = mPhysLayerTxBuffer.front().GetType();
		if(type == DATA) {
			// only write to the physical layer if we have a valid local connection
			if(mpPhys->CanWrite()) {
				mpPhys->AsyncWrite(mPhysLayerTxBuffer.front().mpData, mPhysLayerTxBuffer.front().GetSize());
				LOG_BLOCK(LEV_DEBUG, "Wrote: " << mPhysLayerTxBuffer.front().GetSize());
			}
		}
		else {
			// we only want to handle the remotely sent online/offline message when are not in the process
			// of sending data (waiting for a SendSuccess or SendFailure message)
			if(!mpPhys->IsWriting()){
				this->mPhysLayerTxBuffer.pop();
				this->DoVtoRemoteConnectedChanged(type == REMOTE_OPENED);
			}
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
	this->CheckForVtoWrite();
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
	this->CheckForPhysWrite();
	this->CheckForVtoWrite();
}

void VtoRouter::OnPhysicalLayerOpenFailure()
{
	LOG_BLOCK(LEV_INFO, "Local Connection Open Failed");

	this->SetLocalConnected(false);
	this->CheckForPhysWrite();
	this->CheckForVtoWrite();
}

}
}

/* vim: set ts=4 sw=4: */

