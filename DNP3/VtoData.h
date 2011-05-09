//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or more
// contributor license agreements. See the NOTICE file distributed with this
// work for additional information regarding copyright ownership.  Green Enery
// Corp licenses this file to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance with the
// License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
//

#ifndef __VTO_DATA_H_
#define __VTO_DATA_H_

// from http://pubs.opengroup.org/onlinepubs/009695399/functions/MACRO_BZERO.html
// recommendation is replace with this for portability
#define MACRO_BZERO(b,len) (memset((b), '\0', (len)), (void) 0)

#include <APL/Types.h>

namespace apl {

	namespace dnp {

		/**
		 * Describes the last set value of the setpoint. Like the ControlStatus
		 * data type it is not well supportted and its generally better
		 * practice to use an explict analog.
		 */
		class VtoData
		{
			public:

				const static size_t MAX_SIZE = 255;

				VtoData() :
					mSize(0)
				{}

				VtoData(const uint8_t* aValue, size_t aSize)
				{
					this->Copy(aValue, aSize);
				}

				size_t GetSize() const
				{
					return this->mSize;
				}

				const uint8_t* GetData() const
				{
					return mData;
				}

				void Copy(const uint8_t* aValue, size_t aSize)
				{
					assert(aSize <= MAX_SIZE);
					memcpy(this->mData, aValue, aSize);
					this->mSize = aSize;
				}

			private:

				uint8_t mData[MAX_SIZE];

				size_t mSize;

		};

	}

}

/* vim: set ts=4 sw=4: */

#endif
