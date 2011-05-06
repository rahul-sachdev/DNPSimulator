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
			 * For now, VTO packets are only sent as a single fragment.  We
			 * also will require the receiver to confirm receipt, because I
			 * don't want to have to code reliable delivery into the higher
			 * level protocol.
			 */
			arAPDU.Set(FC_WRITE, true, true, true, false);

			/*
			 * Only grab from the buffer that which will fit into a single APDU
			 * message.  VTO is considered a 'low-priority' traffic flow, so we
			 * don't want to hog the bandwidth with multiple fragments.
			 */
			size_t numObjects = this->mBuffer.Select(
					PC_ALL_EVENTS,
					(arAPDU.Size() - 2) / 300
			);

			/*
			 * If there are no objects to write, skip the remainder.
			 */
			if (numObjects < 0)
				return;

			/*
			 * Loop through the selected data and add corresponding objects to
			 * the arAPDU instance.
			 */
			VtoDataEventIter itr = this->mBuffer.Begin();
			for (size_t i = 0; i < numObjects; ++i, ++itr)
			{
				/* Insert a new object into the APDU message. */
				IndexedWriteIterator obj = arAPDU.WriteIndexed(
						Group112Var0::Inst(),
						itr->mValue.GetSize(),
						itr->mIndex
				);

				/* Set the object index */
				obj.SetIndex(itr->mIndex);

				/* Write the data to the APDU message */
				Group112Var0::Inst()->Write(
						*obj,
						itr->mValue.GetSize(),
						itr->mValue.GetData()
				);
			}
		}

		TaskResult VtoTransmitTask::_OnPartialResponse(const APDU& arAPDU)
		{
			LOG_BLOCK(LEV_WARNING,
					"Ignoring non-FIN response to task: "
					<< this->Name());

			return TR_FAIL;
		}

		TaskResult VtoTransmitTask::_OnFinalResponse(const APDU& arAPDU)
		{
			if (arAPDU.BeginRead().Count() == 0)
			{
				LOG_BLOCK(LEV_WARNING,
						"Unexpected object headers in response: "
						<< this->Name());
			}

			return TR_SUCCESS;
		}

	}
}

/* vim: set ts=4 sw=4: */
