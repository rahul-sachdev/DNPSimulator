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

#include "EventBuffers.h"
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

				/**
				 * Creates a new VtoTransmitTask instance.  The internal buffer
				 * size is set to be an order of magnitude larger than a single
				 * fragment size, to prevent back-ups in the user application.
				 *
				 * @param log		the Logger that the task should use for
				 * 					message reporting
				 * @param fragSize	the size of one fragment
				 *
				 * @return			a new VtoTransmitTask instance
				 */
				VtoTransmitTask(Logger* log, size_t fragSize) :
					MasterTaskBase(log),
					mBuffer(fragSize * 10)
				{}

				/**
				 * A default destructor for the VtoTransmitTask.
				 */
				virtual ~VtoTransmitTask() {}

				/**
				 * Configures the APDU provided for use with DNP3 Virtual
				 * Terminal Objects.  Data is pulled out of the
				 * InsertionOrderedEventBuffer<VtoEvent> instance and is
				 * placed into the APDU.
				 *
				 * The number of Virtual Terminal Objects that are packed into
				 * a single message is calculated based on the size of one
				 * fragment consisting of:
				 *
				 * @code
				 *     1  Application Request Header   2 bytes
				 *
				 *    'N' Virtual Terminal objects   300 bytes each
				 *          Object Type                   2 bytes
				 *          Qualifier                     1 byte
				 *          Range                         1 byte
				 *          Object Prefix Index           1 byte
				 *          Data Octets                   255 bytes
				 * @endcode
				 *
				 * The resulting equation is:
				 *
				 * @code
				 *    Fragment Size = 2 + ( N * [2 + 1 + 1 + 1 + 255] )
				 *    Fragment Size = 2 + ( 300 * N )
				 * @endcode
				 *
				 * Therefore, the number of Virtual Terminal objects that can
				 * fit in a single DNP3 fragment is equal to:
				 *
				 * @code
				 * N = (Fragment Size - 2) / 300
				 * @endcode
				 *
				 * @param arAPDU	the DNP3 message container that will
				 * 					contain the DNP3 Virtual Terminal Objects
				 */
				void ConfigureRequest(APDU& arAPDU);

				/**
				 * Returns the name of the task, as a string.
				 *
				 * @return			the name of the task
				 */
				std::string Name() const
				{
					return "VtoTransmitTask";
				}

				/**
				 * The transmission buffer for Virtual Terminal objects.  Each
				 * object in the buffer is assumed to contain 255-octets of
				 * data, so VtoWriter needs to do a good job of packing things
				 * to maintain efficiency.  Also, Master::TransmitVtoData()
				 * should take care to only write up to the size of the
				 * mBuffer.
				 */
				InsertionOrderedEventBuffer<VtoEvent> mBuffer;

			protected:

				/**
				 * TODO - Any special behavior on a partial response?
				 */
				TaskResult _OnPartialResponse(const APDU& arAPDU);

				/**
				 * TODO - Any special behavior on a final response?
				 */
				TaskResult _OnFinalResponse(const APDU& arAPDU);
		};

	}
}

/* vim: set ts=4 sw=4: */

#endif
