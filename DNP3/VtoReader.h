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

#ifndef __VTO_READER_H_
#define __VTO_READER_H_

#include <map>

#include <APL/Lock.h>
#include <APL/Loggable.h>
#include <APL/DataInterfaces.h>

//#include <boost/cstdint.hpp>

#include "VtoDataInterface.h"
#include "VtoData.h"

namespace apl { 
	namespace dnp {

	/**
	*  Class used to track registered VTO Channels,
	*  assemble VtoData objects back into contigous streams,
	*  and deliver to the correct channel
	*/
	class VtoReader : private Loggable, public ITransactable
	{
		public:

		VtoReader(Logger* apLogger) : 		
			Loggable(apLogger)
		{}

		
		void AddVtoChannel(IVtoCallbacks*);

		void RemoveVtoChannel(IVtoCallbacks*);

		/**  Notifies all registered IVTOCallbacks that space is available
		*/
		void Notify(size_t aAvailableBytes);

		/**
		*   Adds a VTOEvent object to be delivered back to user code.
		*   Must be called from within a transaction block.
		*/
		void Update(const VtoData& arData, boost::uint8_t aChannelId);
								
		private:

		//Implement start and end from ITransaction.

		void _Start();
		void _End();
		
		SigLock mLock;
		typedef std::map<boost::uint8_t, IVtoCallbacks*> ChannelMap;
		ChannelMap mChannelMap;
	};

}}

#endif
