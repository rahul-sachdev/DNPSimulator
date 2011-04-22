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
#ifndef __VTO_DATA_INTERFACES_H_
#define	__VTO_DATA_INTERFACES_H_

#include <APL/DataTypes.h>

#include <assert.h>

namespace apl { namespace dnp {

/**
	The base class for all VTO interface handlers.
 */
class IVtoBase
{
	public:
		IVtoBase() { }
		IVtoBase(byte_t channelId)
		{
			SetChannelId(channelId);
		}

		~IVtoBase() { }

		byte_t GetChannelId()
		{
			return this->channelId;
		}

	protected:
		void SetChannelId(byte_t channelId)
		{
			this->channelId = channelId;
		}

	private:
		byte_t channelId;
};

/**
	The base class for all VTO write interface handlers.
 */
class IVtoWriterBase : protected IVtoBase
{
	public:
		IVtoWriterBase() { }
		IVtoWriterBase(byte_t channelId, size_t reservedOctetCount = 0)
		{
			SetChannelId(channelId);
			SetReservedOctetCount(reservedOctetCount);
		}

		/**
			Writes a stream of data to the remote VTO endpoint.  If the length
			of the stream exceeds the available queue space, the write operation
			will block until more space is available.
			@param data			The data to write to the VTO stream.
			@param length		The length of the data to write (in bytes).
			@param block		If true, the call will block until enough space
								is available in the queue for the data to
								transmit.  If false, the call will write what is
								possible and then return the number of bytes
								transmitted.
			@return				The number of bytes that were successfully
								queued into the VTO transmission queue.
			@throw Exception	If a transmission problem occurs.
		 */
		virtual size_t Send(const byte_t& data, size_t length,
						bool block = true);

		/**
			Returns the minimum number of octets that are to be reserved in the
			DNP3 Application Layer message for VTO data from this stream.
		  */
		size_t GetReservedOctetCount()
		{
			return this->reservedOctetCount;
		}

		/**
			Sets the minimum number of octets that are to be reserved in the
			DNP3 Application Layer message for VTO data from this stream.
			@param count	The number of octets to reserve.
		  */
		void SetReservedOctetCount(size_t count)
		{
			this->reservedOctetCount = count;
		}

	private:
		/**
			The minimum number of octets that are to be reserved in the DNP3
			Application Layer message for VTO data from this stream.
		  */
		size_t reservedOctetCount;
};

/**
	The base class for all VTO read interface handlers.
 */
class IVtoReaderBase : protected IVtoBase
{
	public:
		IVtoReaderBase() { }
		IVtoReaderBase(byte_t channelId)
		{
			SetChannelId(channelId);
		}

		/**
			Reads a stream of data from the remote VTO endpoint.
			@param data			The data received from the VTO stream.
			@param length		The length of the data received (in bytes).
		 */
		virtual void Recv(const byte_t& data, size_t length);
};

class IVtoMasterWriter : protected IVtoWriterBase
{
		IVtoMasterWriter(byte_t channelId, size_t reservedOctetCount = 0)
		{
			SetChannelId(channelId);
			SetReservedOctetCount(reservedOctetCount);
		}

		size_t Send(const byte_t& data, size_t length, bool block = true);
};

class IVtoSlaveWriter : protected IVtoWriterBase
{
		IVtoSlaveWriter(byte_t channelId, size_t reservedOctetCount = 0)
		{
			SetChannelId(channelId);
			SetReservedOctetCount(reservedOctetCount);
		}

		size_t Send(const byte_t& data, size_t length, bool block = true);
};

class IVtoMasterReader : protected IVtoReaderBase
{
		IVtoMasterReader(byte_t channelId)
		{
			SetChannelId(channelId);
		}

		void Recv(const byte_t& data, size_t length);
};

class IVtoSlaveReader : protected IVtoReaderBase
{
		IVtoSlaveReader(byte_t channelId)
		{
			SetChannelId(channelId);
		}

		void Recv(const byte_t& data, size_t length);
};

}} //end namespaces

#endif
