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
#include "SlaveEventBuffer.h"


#include <APL/Exception.h>

namespace apl { namespace dnp {

	SlaveEventBuffer :: SlaveEventBuffer(size_t aMaxBinary, size_t aMaxAnalog, size_t aMaxCounter)
		: mBinaryEvents(aMaxBinary), mAnalogEvents(aMaxAnalog), mCounterEvents(aMaxCounter)
	{	}

	void SlaveEventBuffer :: Update(const Binary& arEvent, PointClass aClass, size_t aIndex)
	{
		mBinaryEvents.Update(arEvent, aClass, aIndex);
	}

	void SlaveEventBuffer :: Update(const Analog& arEvent, PointClass aClass, size_t aIndex)
	{
		mAnalogEvents.Update(arEvent, aClass, aIndex);
	}

	void SlaveEventBuffer :: Update(const Counter& arEvent, PointClass aClass, size_t aIndex)
	{
		mCounterEvents.Update(arEvent, aClass, aIndex);
	}

	size_t SlaveEventBuffer :: NumSelected(DataTypes aType)
	{
		switch(aType){
			case DT_BINARY:
				return mBinaryEvents.NumSelected();
			case DT_ANALOG:
				return mAnalogEvents.NumSelected();
			case DT_COUNTER:
				return mCounterEvents.NumSelected();
			default:
				throw ArgumentException(LOCATION, "Invalid event type");
		}
	}

	size_t SlaveEventBuffer :: NumType(DataTypes aType)
	{
		switch(aType){
			case DT_BINARY:
				return mBinaryEvents.Size();
			case DT_ANALOG:
				return mAnalogEvents.Size();
			case DT_COUNTER:
				return mCounterEvents.Size();
			default:
				throw ArgumentException(LOCATION, "Invalid event type");
		}
	}

	size_t SlaveEventBuffer :: NumSelected()
	{
		return mBinaryEvents.NumSelected() + mAnalogEvents.NumSelected() + mCounterEvents.NumSelected();
	}

	bool SlaveEventBuffer :: IsOverflow()
	{
		return mBinaryEvents.IsOverflown() || mAnalogEvents.IsOverflown() || mCounterEvents.IsOverflown();
	}

	bool SlaveEventBuffer :: HasEventData()
	{
		return mBinaryEvents.NumUnselected() > 0 || mAnalogEvents.NumUnselected() > 0  || mCounterEvents.NumUnselected() > 0 ;
	}

	bool SlaveEventBuffer :: HasClassData(PointClass aClass)
	{
		return mBinaryEvents.HasClassData(aClass) || mAnalogEvents.HasClassData(aClass) || mCounterEvents.HasClassData(aClass);
	}

	size_t SlaveEventBuffer :: Select(DataTypes aType, PointClass aClass, size_t aMaxEvent)
	{
		switch(aType){
			case DT_BINARY:
				return mBinaryEvents.Select(aClass, aMaxEvent);
			case DT_ANALOG:
				return mAnalogEvents.Select(aClass, aMaxEvent);
			case DT_COUNTER:
				return mCounterEvents.Select(aClass, aMaxEvent);
			default:
				throw ArgumentException(LOCATION, "Invalid event type");
		}		
	}

	size_t SlaveEventBuffer :: Select(PointClass aClass, size_t aMaxEvent)
	{
		size_t left = aMaxEvent;
		if ( left > 0 )
			left -= mBinaryEvents.Select(aClass, left);
		if ( left > 0 )
			left -= mAnalogEvents.Select(aClass, left);
		if ( left > 0 )
			left -= mCounterEvents.Select(aClass, left);
		return aMaxEvent - left;
	}

	void SlaveEventBuffer :: ClearWritten()
	{
		mBinaryEvents.ClearWrittenEvents();
		mAnalogEvents.ClearWrittenEvents();
		mCounterEvents.ClearWrittenEvents();
	}
	
	void SlaveEventBuffer :: Deselect()
	{
		mBinaryEvents.Deselect();
		mAnalogEvents.Deselect();
		mCounterEvents.Deselect();
	}

}} //end NS
