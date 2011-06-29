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

#include "MockPhysicalLayerMonitor.h"

#include <boost/test/unit_test.hpp>

#include <APL/IPhysicalLayerAsync.h>
#include <APL/Util.h>
#include <APL/ToHex.h>

#include <iostream>

namespace apl
{

MockPhysicalLayerMonitor::MockPhysicalLayerMonitor(Logger* apLogger, IPhysicalLayerAsync* apPhys, ITimerSource* apTimer, millis_t aOpenRetry) :
	Loggable(apLogger),
	AsyncPhysLayerMonitor(apLogger, apPhys, apTimer, aOpenRetry),
	mOpens(0),
	mCloses(0),
	mOpenFailures(0),
	mNumReads(0),
	mBytesRead(0),
	mBytesWritten(0),
	mLastWriteSize(0),
	mReadBuffer(512),
	mWriteBuffer(0),
	mExpectReadBuffer(0)
{
	mState.push(PLS_CLOSED);
}

void MockPhysicalLayerMonitor::OnPhysicalLayerOpen()
{
	mOpens++;
	mpPhys->AsyncRead(mReadBuffer, mReadBuffer.Size());
}

void MockPhysicalLayerMonitor::OnPhysicalLayerClose()
{
	mCloses++;
}

void MockPhysicalLayerMonitor::OnPhysicalLayerOpenFailure()
{
	mOpenFailures++;
}

void MockPhysicalLayerMonitor::OnStateChange(PhysLayerState aState)
{
	mState.push(aState);
}

void MockPhysicalLayerMonitor::_OnReceive(const boost::uint8_t* apData, size_t aNumBytes)
{
	++mNumReads;
	// we should never receive more than we're expecting
	BOOST_REQUIRE(mExpectReadBuffer.Size() >= mBytesRead + aNumBytes);
	CopyableBuffer expecting(mExpectReadBuffer.Buffer() + mBytesRead, aNumBytes);
	CopyableBuffer read(apData, aNumBytes);
	// check that we're receiving what was written
	BOOST_REQUIRE_EQUAL(expecting, read);
	/*
	const boost::uint8_t* pLast = NULL;
	for(size_t i=0; i<aNumBytes; ++i) {		
		if(i > 0 && (apData[i] != 0) && (apData[i] != (*pLast+1))) {
			int diff = static_cast<int>(apData[i]) - static_cast<int>(*pLast);
			if(diff < 0) diff += 0xAA;
			std::cout << "Sequence discontinuity, " << toHex(pLast,1) << " -> " << toHex(apData+i,1) << " diff: " << diff << " read# " << mNumReads << std::endl;			
		}		
		pLast = apData + i;
	}
	*/

	mBytesRead += aNumBytes;
	LOG_BLOCK(LEV_INFO, "Received " << mBytesRead << " of " << mExpectReadBuffer.Size());
	mpPhys->AsyncRead(mReadBuffer, mReadBuffer.Size());
}

void MockPhysicalLayerMonitor::ExpectData(const CopyableBuffer& arData)
{
	mBytesRead = 0;
	mNumReads = 0;
	mExpectReadBuffer = arData;
}

void MockPhysicalLayerMonitor::WriteData(const CopyableBuffer& arData)
{
	BOOST_REQUIRE(mpPhys->CanWrite());
	mBytesWritten = 0;
	mWriteBuffer = arData;
	this->TransmitNext();
}

void MockPhysicalLayerMonitor::_OnSendSuccess(void)
{
	this->mBytesWritten += this->mLastWriteSize;
	this->TransmitNext();
}

void MockPhysicalLayerMonitor::_OnSendFailure(void)
{
	BOOST_REQUIRE(false);
}

bool MockPhysicalLayerMonitor::NextStateIs(PhysLayerState aState)
{
	if(mState.empty()) return false;
	else {
		PhysLayerState state = mState.front();
		LOG_BLOCK(LEV_INFO, "Saw state: " + ConvertToString(state));
		mState.pop();
		return (state == aState);
	}
}

bool MockPhysicalLayerMonitor::AllExpectedDataHasBeenReceived()
{
	return mBytesRead == mExpectReadBuffer.Size();
}

void MockPhysicalLayerMonitor::TransmitNext()
{
	if(mWriteBuffer.Size() > this->mBytesWritten) {
		size_t remaining = mWriteBuffer.Size() - mBytesWritten;
		size_t toWrite = apl::Min<size_t>(4096, remaining);
		mpPhys->AsyncWrite(mWriteBuffer.Buffer() + mBytesWritten, toWrite);
		this->mLastWriteSize = toWrite;
	}
}

}
