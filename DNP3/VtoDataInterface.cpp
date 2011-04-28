/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership.  Green Enery
 * Corp licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "DNP3/APDU.h"
#include "DNP3/DNPConstants.h"
#include "DNP3/Objects.h"
#include "DNP3/VtoDataInterface.h"

using namespace apl;
using namespace apl::dnp;
using namespace std;

namespace apl {
	namespace dnp {

		SizeByVariationObject* VtoMasterWriter::GetObjectGroupInstance()
		{
			return Group112Var0::Inst();
		}

		SizeByVariationObject* VtoSlaveWriter::GetObjectGroupInstance()
		{
			return Group113Var0::Inst();
		}

		size_t IVtoWriter::Write(const boost::uint8_t* apData, size_t aLength)
		{
			APDU frag;
			size_t numTransmitted = 0;
			SizeByVariationObject *obj = this->GetObjectGroupInstance();
			size_t chunkSize;
			const boost::uint8_t *ptr = NULL;

			/* TODO - should we be allocating an APDU instance ourselves? */
			/* TODO - what's the proper way to handle the sequence number? */
			frag.SetFunction(FC_WRITE);
			frag.SetControl(true, true, false, false, 0);

			/* We need to chunk the arData stream into 255 byte segments */
			while (aLength > 0) {
				/*
				 * Get the size of the next chunk, up to 255 bytes; then
				 * calculate the remaining length in the string.
				 */
				chunkSize = (aLength > 255) ? 255 : aLength;
				aLength -= chunkSize;

				/* Get the index iterator so we can address the byte stream */
				IndexedWriteIterator i = frag.WriteIndexed(obj, chunkSize, this->GetChannelId());
				i.SetIndex(this->GetChannelId());

				/* Find the start of the chunk in the source data */
				ptr = &apData[numTransmitted];
				numTransmitted += chunkSize;

				/* Write the chunk to the object */
				obj->Write(*i, chunkSize, ptr);

				/* Now write the object to the stack */
				/* TODO - how the hell do I do this? */
			}

			/* Return the number of bytes transmitted successfully. */
			return numTransmitted;
		}

	}
}

