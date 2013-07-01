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
#include <opendnp3/APL/Exception.h>
#include <opendnp3/APL/Logger.h>
#include <opendnp3/DNP3/TransportLayer.h>
#include <opendnp3/DNP3/TransportTx.h>

#include <assert.h>
#include <memory.h>
#include <sstream>

using namespace std;

namespace apl
{
namespace dnp
{

TransportTx::TransportTx(Logger* apLogger, TransportLayer* apContext, size_t aFragSize) :
	Loggable(apLogger),
	mpContext(apContext),
	mSeq(0)
{}

void TransportTx::Send(const boost::uint8_t* apData, size_t aNumBytes)
{
	assert(aNumBytes > 0);
	mTpduBuffer.clear();

	size_t numBytesQueued = 0;

	while (numBytesQueued < aNumBytes) {
		// Determine how much size this TPDU needs
		const size_t headerSize = 1;
		const size_t payloadSize = std::min(aNumBytes - numBytesQueued, TL_MAX_TPDU_PAYLOAD);
		const size_t tpduSize = headerSize + payloadSize;

		// Allocate a buffer just large enough
		CopyableBuffer tpdu(tpduSize);
		
		// Copy the application data into the transport layer context
		memcpy(tpdu + 1, apData + numBytesQueued, payloadSize);

		// Determine whether this is the FIRst and/or FINal transport segment
		bool firFlag = (numBytesQueued == 0);
		numBytesQueued += payloadSize;
		bool finFlag = (numBytesQueued == aNumBytes);

		// Fill out the transport layer header
		tpdu[0] = GetHeader(firFlag, finFlag, mSeq);
		LOG_BLOCK(LEV_INTERPRET, "-> " << TransportLayer::ToString(tpdu[0]));

		// Add the transport packet to the outbound queue
		mTpduBuffer.push_back(tpdu);

		// Increment the persistent sequence counter in preparation for the next packet
		mSeq = (mSeq + 1) & 63; // rollover the sequence number at 5 bits
	}

	mpContext->TransmitTPDU(mTpduBuffer);
}

bool TransportTx::SendSuccess()
{
	mTpduBuffer.clear();
	return true;
}

boost::uint8_t TransportTx::GetHeader(bool aFir, bool aFin, int aSeq)
{
	boost::uint8_t hdr = 0;
	if(aFir) hdr |= TL_HDR_FIR;
	if(aFin) hdr |= TL_HDR_FIN;

	// Only the lower 6 bits of the sequence number
	hdr |= TL_HDR_SEQ & aSeq;

	return hdr;
}

}
}

/* vim: set ts=4 sw=4: */

