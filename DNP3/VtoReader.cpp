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

#include "VtoReader.h"

#include <APL/Exception.h>

#include <boost/foreach.hpp>

namespace apl { 
	namespace dnp {

	void VtoReader::AddVtoChannel(IVtoCallbacks* apCallbacks)
	{
		CriticalSection cs(&mLock);

		boost::uint8_t id = apCallbacks->GetChannelId();

		if(mChannelMap.find(id) == mChannelMap.end())
		{
			mChannelMap[id] = apCallbacks;			
		}
		else
		{
			throw new ArgumentException(LOCATION, "Channel already registered: " + id);
		}		
	}
	
	void VtoReader::RemoveVtoChannel(IVtoCallbacks* apCallbacks)
	{
		CriticalSection cs(&mLock);

		boost::uint8_t id = apCallbacks->GetChannelId();

		if(mChannelMap.erase(id) == 0)
			throw new ArgumentException(LOCATION, "Channel not registered: " + id);		
	}

	void VtoReader::Update(const VtoEvent& arEvent)
	{
		assert(this->InProgress());

		// TODO - process the VTO event comparing the index to the map and preparing the data for delivery
	}

	void VtoReader::_Start()
	{
		mLock.Lock();
	}
	
	void VtoReader::_End()
	{
		// TODO - flush recombined streams to output channels here

		mLock.Unlock();		
	}

	void VtoReader::Notify(size_t aAvailableBytes)
	{		
		CriticalSection cs(&mLock);

		for(ChannelMap::iterator i = mChannelMap.begin(); i != mChannelMap.end(); ++i)
		{
			i->second->OnBufferAvailable(aAvailableBytes);
		}
	}
	
}}


