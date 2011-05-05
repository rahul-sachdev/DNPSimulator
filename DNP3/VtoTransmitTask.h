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

#ifndef __VTO_WRITER_TO_BUFFER_TASK_H_
#define __VTO_WRITER_TO_BUFFER_TASK_H_

#include <boost/function.hpp>

#include "MasterTaskBase.h"

using namespace apl;

namespace apl {
	namespace dnp {

		/**
		 * A task to pull data off the VtoWriter queue and transmit it as an
		 * APDU.  The actual work is done by Master::TransmitVtoData().
		 */
		class VtoTransmitTask : public MasterTaskBase
		{
			public:

				VtoTransmitTask(Logger* log) : MasterTaskBase(log)
				{}

				virtual ~VtoTransmitTask() {}

				void ConfigureRequest(APDU& arAPDU);

				std::string Name() const
				{
					return "VtoTransmitTask";
				}

			protected:

				TaskResult _OnPartialResponse(const APDU& arAPDU);

				TaskResult _OnFinalResponse(const APDU& arAPDU);
		};

	}
}

/* vim: set ts=4 sw=4: */

#endif
