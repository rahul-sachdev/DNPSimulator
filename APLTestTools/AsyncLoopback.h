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
#ifndef __ASYNC_LOOPBACK_H_
#define __ASYNC_LOOPBACK_H_

#include <APLTestTools/AsyncTestObjectASIO.h>
#include <APL/AsyncPhysLayerMonitor.h>
#include <APL/CopyableBuffer.h>

namespace apl
{

class AsyncLoopback : public AsyncPhysLayerMonitor
{
public:
	AsyncLoopback(Logger*, IPhysicalLayerAsync*, ITimerSource*, FilterLevel aLevel = LEV_INFO, bool aImmediate = false);

	size_t mBytesWritten;
	size_t mBytesRead;

private:

	CopyableBuffer mRead;
	CopyableBuffer mWrite;	

	void OnStateChange(PhysLayerState) {}

	void _OnReceive(const boost::uint8_t*, size_t);
	void _OnSendSuccess(void);
	void _OnSendFailure(void) {}

	void OnPhysicalLayerOpen(void);
	void OnPhysicalLayerClose(void);

	void StartRead();
};

}

#endif
