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
#ifndef __TRANSPORT_TX_H_
#define __TRANSPORT_TX_H_

#include <opendnp3/APL/CopyableBuffer.h>
#include <opendnp3/APL/Loggable.h>
#include <opendnp3/APL/Types.h>
#include <opendnp3/DNP3/TransportConstants.h>
#include <deque>

namespace apl
{
namespace dnp
{

class TransportLayer;

/**
State/validation for the DNP3 transport layer's send channel.
*/
class TransportTx : public Loggable
{
public:
	TransportTx(Logger*, TransportLayer*, size_t aFragSize);


	void Send(const boost::uint8_t*, size_t); // A fresh call to Send() will reset the state
	bool SendSuccess();


	static boost::uint8_t GetHeader(bool aFir, bool aFin, int aSeq);

private:
	TransportLayer* mpContext;

	/**
	 * A persistent sequence counter that is incremented by
	 * TransportTx::Send().
	 */
	int mSeq;

	/**
	 * A queue of transport layer packets that are being processed
	 * by the link layer.  This queue is built up by
	 * TransportTx::Send() and cleared away by
	 * TransportTx::SendSuccess().  Each transport packet is encoded
	 * as a CopyableBuffer of the exact length required.
	 */
	std::deque<CopyableBuffer> mTpduBuffer;
};

}
}

#endif
