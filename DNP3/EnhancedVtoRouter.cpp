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

#include "EnhancedVtoRouter.h"

namespace apl
{
namespace dnp
{

EnhancedVtoRouter::EnhancedVtoRouter(const VtoRouterSettings& arSettings, Logger* apLogger, IVtoWriter* apWriter, IPhysicalLayerAsync* apPhysLayer, ITimerSource* apTimerSrc) :
	Loggable(apLogger),
	VtoRouter(arSettings, apLogger, apWriter, apPhysLayer, apTimerSrc),
	mRemoteConnected(false),
	mLocalConnected(false)
{

}

std::string EnhancedVtoRouter::GetConnectionString(bool aOpen)
{
	return aOpen ? "OPEN" : "CLOSED";
}

void EnhancedVtoRouter::DoVtoRemoteConnectedChanged(bool aOpened)
{
	if(mRemoteConnected != aOpened) {
		LOG_BLOCK(LEV_INFO, "Remote connection: " << GetConnectionString(aOpened));
		mRemoteConnected = aOpened;
		this->HandleVtoRemoteConnectedChanged();
	}
}

void EnhancedVtoRouter::SetLocalConnected(bool aConnected)
{
	if(mLocalConnected != aConnected) {
		LOG_BLOCK(LEV_INFO, "Local connection: " << GetConnectionString(aConnected));
		mLocalConnected = aConnected;

		this->HandleSetLocalConnected();

		mVtoTxBuffer.push_back(VtoMessage(aConnected ? VTODT_REMOTE_OPENED : VTODT_REMOTE_CLOSED));
	}
}

void EnhancedVtoRouter::FlushBuffers()
{
	// clear out all of the data when we close the local connection

	while(mPhysLayerTxBuffer.size() > 0) {
		LOG_BLOCK(LEV_WARNING, "Tossing data: " << this->mPhysLayerTxBuffer.front().GetType() << " size: " << this->mPhysLayerTxBuffer.front().GetSize());
		this->mPhysLayerTxBuffer.pop();
	}

}

void EnhancedVtoRouter::StopAndFlushBuffers()
{
	FlushBuffers();

	this->DoStop();
}

/*****************************************
 * Server socket specific implementation
 *************************************/

ServerSocketVtoRouter::ServerSocketVtoRouter(const VtoRouterSettings& arSettings, Logger* apLogger, IVtoWriter* apWriter, IPhysicalLayerAsync* apPhysLayer, ITimerSource* apTimerSrc) :
	Loggable(apLogger),
	EnhancedVtoRouter(arSettings, apLogger, apWriter, apPhysLayer, apTimerSrc)
{
	// This type of router always runs
	this->DoStart();
}

void ServerSocketVtoRouter::HandleVtoRemoteConnectedChanged()
{
	if(mRemoteConnected) {

	}
	else {
		// if the remote side has closed we should close our
		// local connection and then prepare for a new one
		this->FlushBuffers();
		this->Reconnect();
	}

}

void ServerSocketVtoRouter::HandleSetLocalConnected()
{
	if(!mLocalConnected) this->FlushBuffers();
}

/*****************************************
 * Client socket specific implementation
 *************************************/
ClientSocketVtoRouter::ClientSocketVtoRouter(const VtoRouterSettings& arSettings, Logger* apLogger, IVtoWriter* apWriter, IPhysicalLayerAsync* apPhysLayer, ITimerSource* apTimerSrc) :
	Loggable(apLogger),
	EnhancedVtoRouter(arSettings, apLogger, apWriter, apPhysLayer, apTimerSrc)
{

}

void ClientSocketVtoRouter::HandleVtoRemoteConnectedChanged()
{
	if(mRemoteConnected) {
		// pretend we are online, so the other side sees a "connected"
		// message. If we succeed in connecting the second SetLocalConnected(true)
		// call will do nothing, if we fail the SetLocalConnected(false) call
		// will cause a "disconnected" message to be sent so the server side
		// can close the connection. Effectivley we tunnel the "connection
		// refused" behavior as a "remote server terminated connection"
		this->SetLocalConnected(true);
		this->DoStart();
	}
	else {
		// always stop the local connection attempts if the remote is disconnected
		this->StopAndFlushBuffers();
	}
}

void ClientSocketVtoRouter::HandleSetLocalConnected()
{
	// we shouldn't automatically reconnect when the connection drops
	if(!mLocalConnected) this->StopAndFlushBuffers();
}

}
}

/* vim: set ts=4 sw=4: */

