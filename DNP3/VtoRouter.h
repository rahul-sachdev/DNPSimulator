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

#ifndef __VTO_ROUTER_H_
#define __VTO_ROUTER_H_

#include <queue>

#include <APL/IHandlerAsync.h>
#include <APL/AsyncPhysLayerMonitor.h>
#include <APL/ShiftableBuffer.h>

#include "VtoDataInterface.h"
#include "CleanupHelper.h"

namespace apl
{

class IPhysicalLayerAsync;
class ITimerSource;

namespace dnp
{

class VtoReader;
class VtoWriter;
struct VtoRouterSettings;

/**
 * Class used to route data between a VTO channel (made up of both a
 * VtoWriter and VtoReader instance) and an IPhysicalLayerAsync
 * instance.
 *
 * @code
 *                                           +--> [VtoReader]
 *                                           |
 * [IPhysicalLayerAsync] <--> [VtoRouter] <--+
 *                                           |
 *                                           +--> [VtoWriter]
 * @endcode
 *
 * The VtoRouter instance provides the necessary IVtoCallbacks hooks
 * that the VtoReader will use.
 */
class VtoRouter : public AsyncPhysLayerMonitor, public IVtoCallbacks, public CleanupHelper
{
public:

	/**
	 * Create a new VtoRouter instance.  Prior to using this
	 * instance, make sure to register both a IPhysicalLayerAsync
	 * and a VtoWriter using SetPhysicalLayer() and
	 * SetVtoWriter(), respectively.
	 *
	 * @param apLogger			the Logger that the instance
	 * 							should use for log messages
	 * @param aChannelId		the DNP3 Virtual Terminal port
	 * 							(channel id)
	 *
	 * @return					a new VtoRouter instance
	 */
	VtoRouter(const VtoRouterSettings& arSettings, Logger* apLogger, IVtoWriter* apWriter, IPhysicalLayerAsync* apPhysLayer, ITimerSource* apTimerSrc);

	/**
	 * when we try to stop the router we call this thread safe function which sets a flag and
	 * then posts a shutdown request to mpTimerSrc.
	 */
	void StopRouter();


	/**
	 * Receives data from the VTO channel and forwards it to the
	 * IPhysicalLayerAsync instance associated with this VtoRouter.
	 * Implements IVtoCallbacks::OnVtoDataReceived().
	 *
	 * @param apData		The data received from the VTO stream
	 * @param aLength		The length of the data received (in
	 *						bytes)
	 */
	void OnVtoDataReceived(const boost::uint8_t* apData,
	                       size_t aLength);

	/**
	 * When the remote end indicates a connection state change we need to toggle
	 * our connection state and setup for a new connection. Behavior will be
	 * different depending on the mStartLocal setting. Posted to mpTimerSrc.
	 */
	void OnVtoRemoteConnectedChanged(bool aOpened);

	/**
	 * When the dnp stack changes state to offline we will shutdown any our local physical
	 * layer. When it comes online we may start our local physical layer, depending on the
	 * mStartLocal setting. Posted to mpTimerSrc.
	 */
	void OnDnpConnectedChanged(bool aConnected);

	/**
	 * Called when the VTO data buffer size changes (startup and
	 * successuly transmission).
	 */
	void OnBufferAvailable();

protected:

	void CheckForPhysRead();
	void CheckForPhysWrite();

	void CheckForVtoWrite();

	// Implement AsyncPhysLayerMonitor

	void OnPhysicalLayerOpen();

	void OnPhysicalLayerClose();

	void OnPhysicalLayerOpenFailure();

	/**
	 * Receives data from the physical layer and forwards it to the
	 * VtoWriter.
	 *
	 * @param apData		The data received from the physical
	 * 						layer
	 * @param aLength		The length of the data received (in
	 *						bytes)
	 */
	void _OnReceive(const boost::uint8_t* apData, size_t aLength);

	/**
	 * Implements IUpperLayer::_OnSendSuccess(), which was
	 * inherited via IHandlerAsync.  Called when an asynchronous
	 * transmission to the physical layer was successful.
	 */
	void _OnSendSuccess();

	/**
	 * Implements IUpperLayer::_OnSendFailure(), which was
	 * inherited via IHandlerAsync.  Called when an asynchronous
	 * transmission to the physical layer was not successful.
	 */
	void _OnSendFailure();

	/**
	 * Implement AsyncPhysMonitor::OnStateChange
	 */
	void OnStateChange(IPhysMonitor::State);

	void DoStart();
	void DoStop();

	void SetLocalConnected(bool aConnected);

	/**
	 * actually do the work for the external calls
	 */
	void DoStopRouter();
	void DoVtoRemoteConnectedChanged(bool aOpened);
	void DoDnpConnectedChanged(bool aConnected);

private:

	/**
	 * The VtoWriter instance that will be used to send the data
	 * that is received by the IPhysicalLayerAsync instance to the
	 * VTO endpoint.
	 */
	IVtoWriter* mpVtoWriter;

	/**
	 * The transmit buffer for Vto -> physical layer.  The data that
	 * is put into this buffer was originally received via VTO.
	 */
	std::queue<VtoData> mPhysLayerTxBuffer;

	/**
	 * The transmit buffer for physical layer -> Vto. The data that is put
	 * into this buffer was originally received from the physical layer.
	 */
	ShiftableBuffer mVtoTxBuffer;

	bool mDnpConnected;
	bool mRemoteConnected;
	bool mLocalConnected;

	bool mPermanentlyStopped;
	bool mStarted;
	bool mCleanedup;

	/**
	 * determines if we should open our physical layer or wait for the remote
	 * side to indicate the other side is connected.
	 * TODO: refactor this class into abstract base class with 2 implementations
	 * instead of flag
	 */
	bool mStartLocal;

	bool mDisableExtensions;
};

}
}

/* vim: set ts=4 sw=4: */

#endif

