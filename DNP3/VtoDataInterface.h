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
#ifndef __VTO_DATA_INTERFACE_H_
#define	__VTO_DATA_INTERFACE_H_

#include <APL/DataTypes.h>

#include <assert.h>

namespace apl { namespace dnp {

/**
	The base class for all VTO interface handlers.
 */
class IVtoBase
{
	public:
		
		IVtoBase(byte_t aChannelId) : mChannelId(aChannelId)
		{
		}

		//~IVtoBase() { }

		byte_t GetChannelId()
		{
			return this->mChannelId;
		}	

	private:
		IVtoBase() {}
		byte_t mChannelId;
};

/**
	IVTOWriter is returned by the stack for write operations to a Vto stream.
	The Write() function should be used in conjunction with the OnBufferAvailable()
	callback on the IVTOCallbacks interface provided to the stack.
 */
class IVtoWriter : protected IVtoBase
{
	public:
		
		IVtoWriter(byte_t aChannelId, size_t aReservedOctetCount = 0) :  
		  IVtoBase(aChannelId),
		  mReservedOctetCount(aReservedOctetCount)
		{			
			
		}

		/**
			Writes a stream of data to the remote VTO endpoint.

			@param arData		The data to write to the VTO stream.
			@param aLength		The length of the data to write (in bytes).
			
			@return				The number of bytes that were successfully
								queued into the VTO transmission queue. 
								This number may be less than the length request
								if the buffer has insufficient space.
			
		 */
		virtual size_t Write(const byte_t& arData, size_t aLength) = 0;
						

		/**
			Returns the minimum number of octets that are to be reserved in the
			DNP3 Application Layer message for VTO data from this stream.
		  */
		size_t GetReservedOctetCount()
		{
			return this->mReservedOctetCount;
		}
		

	private:
		
		/**
			The minimum number of octets that are to be reserved in the DNP3
			Application Layer message for VTO data from this stream.
		  */
		size_t mReservedOctetCount;
};

/**
	Receives data from the stack for a particular channel and is notified
	when buffer space becomes available.
 */
class IVtoCallbacks : protected IVtoBase
{
	public:

		IVtoCallbacks(byte_t aChannelId) : IVtoBase(aChannelId)
		{
		}

		/**
			Called when data arrives from stack and needs to be handled.

			@param arData			The data received from the VTO stream.
			@param aLength			The length of the data received (in bytes).
		 */
		virtual void OnDataReceived(const byte_t& arData, size_t aLength) = 0;

		/**
			Called when the Vto data buffer size changes (startup and successuly transmission)
			
			@param aSize			Available space in the buffer in bytes
		*/
		virtual void OnBufferAvailable(size_t aSize) = 0;
};

}} //end namespaces

#endif
