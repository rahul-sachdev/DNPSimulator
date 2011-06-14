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

#include "EnhancedVto.h"

#include "VtoData.h"

namespace apl
{

namespace dnp
{

//spells out opendnpvto
const char EnhancedVto::MAGIC_BYTES[MAGIC_BYTES_SIZE] = {'o', 'p', 'e', 'n', 'd', 'n', 'p', 'v', 't', 'o'};

VtoData EnhancedVto::CreateVtoData(bool aLocalVtoConnectionOpened, boost::uint8_t aChannelId)
{
	VtoData vto(2);
	vto.mpData[0] = aChannelId;
	vto.mpData[1] = aLocalVtoConnectionOpened ? 0 : 1;
	return vto;
}

}

}

/* vim: set ts=4 sw=4: */


