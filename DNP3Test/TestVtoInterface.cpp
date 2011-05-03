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

#include <DNP3/VtoData.h>
#include <DNP3/VtoDataInterface.h>
#include <DNP3/VtoReader.h>
#include <DNP3/VtoWriter.h>

#include <DNP3/DNPConstants.h>
#include <APL/DataTypes.h>
#include <APLTestTools/BufferHelpers.h>

using namespace std;
using namespace apl;
using namespace apl::dnp;

#define MAX_SIZE					(255)

BOOST_AUTO_TEST_SUITE(VtoInterfaceTests)
	BOOST_AUTO_TEST_CASE(VtoWriteSeveral)
	{
		const size_t numChunks = 10;
		const size_t emptySize = numChunks * MAX_SIZE;
		VtoWriter writer(numChunks);

		const size_t dataSize = 5;
		uint8_t data[dataSize];

		size_t len;

		for (size_t i = 0; i < dataSize; ++i)
			data[i] = i % 255;

		/* Check the default size to make sure we have full space available */
		BOOST_REQUIRE_EQUAL(writer.NumBytesAvailable(), emptySize);

		/* Write no data, size should remain the same */
		len = writer.Write(data, 0, 1);
		BOOST_REQUIRE_EQUAL(len, 0);
		BOOST_WARN_EQUAL(writer.NumBytesAvailable(), emptySize);

		/* Write a partial frame */
		len = writer.Write(data, 5, 2);
		BOOST_REQUIRE_EQUAL(len, 5);
		BOOST_WARN_EQUAL(writer.NumBytesAvailable(), emptySize - 5);

		/* Write a complete frame */
		len = writer.Write(data, MAX_SIZE, 3);
		BOOST_REQUIRE_EQUAL(len, MAX_SIZE);
		BOOST_WARN_EQUAL(writer.NumBytesAvailable(), emptySize - 5 - MAX_SIZE);

		/* Write a complete and a partial frame */
		len = writer.Write(data, MAX_SIZE + 5, 1);
		BOOST_REQUIRE_EQUAL(len, MAX_SIZE + 5);
		BOOST_WARN_EQUAL(writer.NumBytesAvailable(), emptySize - 5 - MAX_SIZE - (MAX_SIZE + 5));
	}

	BOOST_AUTO_TEST_CASE(VtoWriteOverflow)
	{
		const size_t numChunks = 3;
		const size_t emptySize = numChunks * MAX_SIZE;
		VtoWriter writer(numChunks);

		const size_t dataSize = MAX_SIZE * 10;
		uint8_t data[dataSize];

		size_t len;

		for (size_t i = 0; i < dataSize; ++i)
			data[i] = i % 255;

		/* Check the default size to make sure we have full space available */
		BOOST_REQUIRE_EQUAL(writer.NumBytesAvailable(), emptySize);

		/* Write all data, only some should get written */
		len = writer.Write(data, dataSize, 1);
		BOOST_REQUIRE_EQUAL(len, emptySize);
		BOOST_REQUIRE(len != dataSize);
		BOOST_REQUIRE_EQUAL(writer.NumBytesAvailable(), 0);
	}
BOOST_AUTO_TEST_SUITE_END()

/* vim: set ts=4 sw=4: */
