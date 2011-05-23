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

#include <APL/Exception.h>

#include "VtoRouter.h"

using apl::IPhysicalLayerAsync;
using apl::Logger;
using apl::NotImplementedException;
using apl::dnp::VtoRouter;

VtoRouter::VtoRouter(Logger* apLogger, boost::uint8_t aChannelId, VtoWriter* apVtoWriter, IPhysicalLayerAsync* apPhysLayer) :
	IVtoCallbacks(aChannelId),
	IHandlerAsync(apLogger),
	mpVtoWriter(apVtoWriter),
	mpPhysLayer(apPhyslayer)
{
	this->mpPhysLayer->SetHandler(this);
}

void VtoRouter::OnDataReceived(const boost::uint8_t* apData, size_t aLength)
{
	/* TODO - write the data to the IPhysicalLayerAsync instance */
	/* TODO - how do we do this? */
	throw NotImplementedException(LOCATION);
}

void VtoRouter::_OnReceive(const boost::uint8_t* apData, size_t aLength)
{
	this->mpVtoWriter->Write(apData, aLength, this->GetChannelId());
}

void VtoRouter::_OnSendSuccess()
{
	/* TODO - anything to do? */
	throw NotImplementedException(LOCATION);
}

void VtoRouter::_OnSendFailure()
{
	/* TODO - anything to do? */
	throw NotImplementedException(LOCATION);
}

void VtoRouter::_OnOpenFailure()
{
	/* TODO - anything to do? */
	throw NotImplementedException(LOCATION);
}

/* vim: set ts=4 sw=4: */

