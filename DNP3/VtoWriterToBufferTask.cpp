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
#include "VtoWriterToBufferTask.h"

using namespace apl;
using namespace apl::dnp;

namespace apl {
	namespace dnp {

		void VtoWriterToBufferTask::ConfigureRequest(APDU& arAPDU)
		{

		}

		TaskResult VtoWriterToBufferTask::_OnPartialResponse(const APDU& arAPDU)
		{
			LOG_BLOCK(LEV_WARNING,
					"Ignoring non-FIN response to task: "
					<< this->Name());

			return TR_FAIL;
		}

		TaskResult VtoWriterToBufferTask::_OnFinalResponse(const APDU& arAPDU)
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
