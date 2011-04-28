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

#include "VtoEnabledStack.h"

#include <APL/Exception.h>

#include <boost/foreach.hpp>

namespace apl { 
	namespace dnp {

	IVtoWriter* VtoEnabledStack::AddChannel(IVtoCallbacks* apCallbacks)
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

		return &mQueue;
	}
	
	void VtoEnabledStack::RemoveChannel(IVtoCallbacks* apCallbacks)
	{
		CriticalSection cs(&mLock);

		boost::uint8_t id = apCallbacks->GetChannelId();

		if(mChannelMap.erase(id) == 0)
			throw new ArgumentException(LOCATION, "Channel not registered: " + id);
		
	}

	void VtoEnabledStack::NotifyOfSpace()
	{
		CriticalSection cs(&mLock);
		for(ChannelMap::iterator i = mChannelMap.begin(); i != mChannelMap.end(); ++i)
		{
			i->second->OnBufferAvailable(mQueue.NumBytesAvailable());
		}
	}
	
}}


