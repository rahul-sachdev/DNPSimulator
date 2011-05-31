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
#include <boost/test/unit_test.hpp>
#include <APLTestTools/TestHelpers.h>

#include <APL/Exception.h>
#include <DNP3/SlaveEventBuffer.h>
#include <DNP3/VtoWriter.h>
#include <limits>

using namespace std;
using namespace apl;
using namespace apl::dnp;

BOOST_AUTO_TEST_SUITE(SlaveEventBufferSuite)
	void PushEvents(SlaveEventBuffer& b, size_t aNumEvent, size_t aNumIndices)
	{
		for(size_t i=0; i<aNumEvent; ++i) {
			b.Update(Analog(i), PC_CLASS_1, i%aNumIndices);
		}
	}

	template <class T>
	void OverflowTest(const T& first, const T& second)
	{
		EventMaxConfig cfg(1, 1, 1, 0);
		SlaveEventBuffer b(cfg);
		b.Update(first, PC_CLASS_1, 0);
		BOOST_REQUIRE_FALSE(b.IsOverflow());
		BOOST_REQUIRE(b.HasEventData());
		BOOST_REQUIRE_EQUAL(b.NumType(Convert(T::MeasEnum)), 1);
		b.Update(second, PC_CLASS_1, 1);
		BOOST_REQUIRE(b.IsOverflow());
	}

	BOOST_AUTO_TEST_CASE(AnalogInsertion)
	{
		const size_t NUM_EVENT = 100;
		const size_t NUM_INDICES = 10;

		EventMaxConfig cfg(0, NUM_INDICES, 0, 0);

		SlaveEventBuffer b(cfg);

		PushEvents(b, NUM_EVENT, NUM_INDICES); //push lots of events but only
		BOOST_REQUIRE_FALSE(b.IsOverflow());
		BOOST_REQUIRE_EQUAL(b.NumType(BT_ANALOG), NUM_INDICES);

		b.Select(BT_ANALOG, PC_CLASS_1); //select all the events
		BOOST_REQUIRE_EQUAL(b.NumType(BT_ANALOG), NUM_INDICES);
		PushEvents(b, NUM_EVENT, NUM_INDICES);
		BOOST_REQUIRE_FALSE(b.IsOverflow());
		BOOST_REQUIRE_EQUAL(b.NumType(BT_ANALOG), 2*NUM_INDICES);

		//now deselect
		b.Deselect();
		BOOST_REQUIRE_EQUAL(b.NumType(BT_ANALOG), NUM_INDICES);
		BOOST_REQUIRE_FALSE(b.IsOverflow()); //still shouldn't be
	}

	BOOST_AUTO_TEST_CASE(OverflowAnalog)
	{
		OverflowTest(Analog(5), Analog(6));
	}

	BOOST_AUTO_TEST_CASE(OverflowBinary)
	{
		OverflowTest(Binary(true), Binary(false));
	}

	BOOST_AUTO_TEST_CASE(OverflowCounter)
	{
		OverflowTest(Counter(1), Counter(6));
	}

	BOOST_AUTO_TEST_CASE(ClassSelect)
	{
		EventMaxConfig cfg(10, 10, 10, 0);
		SlaveEventBuffer b(cfg);
		b.Update(Binary(true), PC_CLASS_1, 0);
		b.Update(Binary(false), PC_CLASS_1, 0);
		b.Update(Analog(5), PC_CLASS_1, 0);
		b.Update(Analog(6), PC_CLASS_1, 1);
		b.Update(Counter(1), PC_CLASS_1, 0);
		b.Update(Counter(3), PC_CLASS_1, 1);

		b.Select(PC_CLASS_1, 1);
		BOOST_REQUIRE_EQUAL(b.NumSelected(BT_BINARY), 1);
		b.Deselect();

		b.Select(PC_CLASS_1, 3);
		BOOST_REQUIRE_EQUAL(b.NumSelected(BT_BINARY), 2);
		BOOST_REQUIRE_EQUAL(b.NumSelected(BT_ANALOG), 1);
		b.Deselect();

		b.Select(PC_CLASS_1, 5);
		BOOST_REQUIRE_EQUAL(b.NumSelected(BT_BINARY), 2);
		BOOST_REQUIRE_EQUAL(b.NumSelected(BT_ANALOG), 2);
		BOOST_REQUIRE_EQUAL(b.NumSelected(BT_COUNTER), 1);
		b.Deselect();

		b.Select(PC_CLASS_1, 6);
		BOOST_REQUIRE_EQUAL(b.NumSelected(BT_BINARY), 2);
		BOOST_REQUIRE_EQUAL(b.NumSelected(BT_ANALOG), 2);
		BOOST_REQUIRE_EQUAL(b.NumSelected(BT_COUNTER), 2);

		BOOST_REQUIRE(!b.IsFull(BT_BINARY));
		BOOST_REQUIRE(!b.IsFull(BT_ANALOG));
		BOOST_REQUIRE(!b.IsFull(BT_COUNTER));
	}

	BOOST_AUTO_TEST_CASE(SlaveVtoEvents)
	{
		size_t vtoBufferSize = 2;
		VtoWriter writer(vtoBufferSize + 1);
		EventMaxConfig cfg( 0, 0, 0, vtoBufferSize);
		SlaveEventBuffer b(cfg);
		VtoEvent info;

		/* Initialize the data stream to a pseudo-random sequence */
		size_t dataSize = 255;
		boost::uint8_t data[dataSize];
		for (size_t i = 0; i < dataSize; ++i)
		{
			data[i] = i;
		}

		/*
		 * Add the data to the VtoWriter, as three different virtual
		 * channels
		 */
		for (size_t i = 0; i < (vtoBufferSize + 1); ++i)
		{
			writer.Write(data, dataSize, i);
		}

		/* Fill up the SlaveEventBuffer with VTO data */
		for (size_t i = 0; i < vtoBufferSize; ++i)
		{
			BOOST_REQUIRE(!b.IsFull(BT_VTO));
			writer.Read(info);
			b.Update(info);
		}

		BOOST_REQUIRE(b.IsFull(BT_VTO));
		BOOST_REQUIRE_EQUAL(b.NumType(BT_VTO), vtoBufferSize);

		/* Select a morsel of data and clear it */
		BOOST_REQUIRE_EQUAL(b.Select(PC_CLASS_1, 1), 1);
		VtoDataEventIter itr;
		b.Begin(itr);
		itr->mWritten = true;
		b.ClearWritten();

		/* The buffer should now have one more space left */
		BOOST_REQUIRE(!b.IsFull(BT_VTO));
		BOOST_REQUIRE_EQUAL(b.NumType(BT_VTO), vtoBufferSize - 1);
		writer.Read(info);
		b.Update(info);

		/* The buffer should now be full again */
		BOOST_REQUIRE(b.IsFull(BT_VTO));
		BOOST_REQUIRE_EQUAL(b.NumType(BT_VTO), vtoBufferSize);

		/* Clear out the buffer entirely */
		b.Select(PC_CLASS_1);
		b.Begin(itr);
		for (size_t i = 0; i < vtoBufferSize; i++)
		{
			itr->mWritten = true;
			itr++;
		}
		b.ClearWritten();

		BOOST_REQUIRE(!b.IsFull(BT_VTO));
		BOOST_REQUIRE_EQUAL(b.NumType(BT_VTO), 0);
	}
BOOST_AUTO_TEST_SUITE_END()
