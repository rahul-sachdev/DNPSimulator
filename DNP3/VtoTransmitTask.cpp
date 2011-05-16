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

#include <APL/Logger.h>

#include "APDU.h"
#include "VtoTransmitTask.h"

using namespace apl;
using namespace apl::dnp;

namespace apl {
	namespace dnp {

		void VtoTransmitTask::ConfigureRequest(APDU& arAPDU)
		{
			/*
			 *  Masters never request confirmed data. The response from the
			 *  slave is all that's required for reliable delivery.
			 */
			arAPDU.Set(FC_WRITE);

			/* Get all of the data objects in the buffer. */
			size_t numObjects = this->mBuffer.Select(PC_ALL_EVENTS);

			/* If there are no objects to write, skip the remainder. */
			if (numObjects < 0)
			{
				return;
			}

			/*
			 * Loop through the selected data and add corresponding objects to
			 * the arAPDU instance.
			 */
			VtoDataEventIter vto = this->mBuffer.Begin();

			for (size_t i = 0; i < numObjects; ++i)
			{
				/* Insert a new object into the APDU message. */
				IndexedWriteIterator itr = arAPDU.WriteIndexed(
						Group112Var0::Inst(),
						vto->mValue.GetSize(),
						vto->mIndex
				);

				/*
				 * Check to see if the APDU fragment has enough room for the
				 * data segment.  If the fragment is full, return out of this
				 * function and let the fragment send.
				 */
				if (itr.IsEnd())
				{
					return;
				}

				/* Set the object index */
				itr.SetIndex(vto->mIndex);

				/* Write the data to the APDU message */
				Group112Var0::Inst()->Write(
						*itr,
						vto->mValue.GetSize(),
						vto->mValue.GetData()
				);

				/* Mark the data segment as being written */
				vto->mWritten = true;

				/* Move to the next data segment in the buffer */
				++vto;
			}
		}

		TaskResult VtoTransmitTask::_OnPartialResponse(const APDU& arAPDU)
		{
			LOG_BLOCK(LEV_ERROR,
					"Non fin responses not allowed for task: "
					<< this->Name());

			return TR_CONTINUE;
		}

		TaskResult VtoTransmitTask::_OnFinalResponse(const APDU& arAPDU)
		{
			/* Remove the written data from the buffer */
			this->mBuffer.ClearWrittenEvents();

			return TR_SUCCESS;
		}

		void VtoTransmitTask::OnFailure()
		{
			/* Put the written data back onto the buffer */
			this->mBuffer.Deselect();
		}

	}
}

/* vim: set ts=4 sw=4: */
