//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  Green Enery Corp licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//

#include "AsyncLoopback.h"

#include <APL/IPhysicalLayerAsync.h>

namespace apl
{

AsyncLoopback::AsyncLoopback(Logger* apLogger, IPhysicalLayerAsync* apPhys, ITimerSource* apTimerSrc, FilterLevel aLevel, bool aImmediate) :
	Loggable(apLogger),
	AsyncPhysLayerMonitor(apLogger, apPhys, apTimerSrc, 5000),
	mBytesWritten(0),
	mBytesRead(0),
	mRead(1024),
	mWrite(mRead)	
{

}

void AsyncLoopback::StartRead()
{
	mpPhys->AsyncRead(mRead, mRead.Size());
}

void AsyncLoopback::_OnReceive(const boost::uint8_t* apData, size_t aNumBytes)
{
	mBytesRead += aNumBytes;
	if(mpPhys->CanWrite()) {
		memcpy(mWrite, mRead, aNumBytes);
		mpPhys->AsyncWrite(mWrite, aNumBytes);
		this->StartRead();
	}
}

void AsyncLoopback::_OnSendSuccess(void)
{

}

void AsyncLoopback::OnPhysicalLayerOpen(void)
{
	LOG_BLOCK(LEV_INFO, "Opened");
	this->StartRead();
}

void AsyncLoopback::OnPhysicalLayerClose(void)
{
	LOG_BLOCK(LEV_INFO, "Closed");
}

}

