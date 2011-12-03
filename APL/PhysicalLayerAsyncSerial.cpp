//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  Green Enery Corp licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//

#include "PhysicalLayerAsyncSerial.h"

#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "Exception.h"
#include "IHandlerAsync.h"
#include "Logger.h"
#include "ASIOSerialHelpers.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using namespace std;

namespace apl
{

PhysicalLayerAsyncSerial::PhysicalLayerAsyncSerial(Logger* apLogger, boost::asio::io_service* apIOService, const SerialSettings& arSettings)
	: PhysicalLayerAsyncASIO(apLogger, apIOService)
	, mSettings(arSettings)
	, mpService(apIOService)
	, mPort(*apIOService)
{}

void PhysicalLayerAsyncSerial::CaptureLog(const std::string& prefix, const std::string& message)
{
	if (this->mSettings.mCaptureEnabled) {
		std::ofstream ofs(this->mSettings.mCaptureFilename.c_str(), std::ios_base::out | std::ios_base::app);
		ofs << prefix << " " << message << std::endl;
	}
}

void PhysicalLayerAsyncSerial::CaptureLog(const std::string& prefix, const boost::uint8_t* buf, size_t len)
{
	if (this->mSettings.mCaptureEnabled) {
		std::ostringstream os;
		os << setfill('0');
		os << "[len=" << setw(4) << len << "]";
		os << hex << uppercase;
		for (size_t i = 0; i < len; ++i)
			os << " " << setw(2) << static_cast<int>(buf[i]);
		this->CaptureLog(prefix, os.str());
	}
}

void PhysicalLayerAsyncSerial::DoOpen()
{
	boost::system::error_code ec;
	mPort.open(mSettings.mDevice, ec);

	//use post to simulate an async open operation
	if (!ec)
		asio_serial::Configure(mSettings, mPort, ec);

	this->CaptureLog("MGMT:", ">>> connection opened <<<");

	mpService->post(bind(&PhysicalLayerAsyncSerial::OnOpenCallback, this, ec));
}

void PhysicalLayerAsyncSerial::DoClose()
{
	boost::system::error_code ec;
	mPort.close(ec);

	this->CaptureLog("MGMT:", ">>> connection closed <<<");

	if (ec)
		LOG_BLOCK(LEV_WARNING, ec.message());
}

void PhysicalLayerAsyncSerial::DoOpenSuccess()
{
	LOG_BLOCK(LEV_INFO, "Port successfully opened");
}

void PhysicalLayerAsyncSerial::OnReadCallbackHelper(const boost::system::error_code& arError, boost::uint8_t* apBuff, size_t aSize)
{
	this->CaptureLog("RX:", apBuff, aSize);
	this->OnReadCallback(arError, apBuff, aSize);
}

void PhysicalLayerAsyncSerial::DoAsyncRead(boost::uint8_t* apBuffer, size_t aMaxBytes)
{
	mPort.async_read_some(buffer(apBuffer, aMaxBytes),
	                      boost::bind(&PhysicalLayerAsyncSerial::OnReadCallbackHelper,
	                                  this,
	                                  boost::asio::placeholders::error,
	                                  apBuffer,
	                                  boost::asio::placeholders::bytes_transferred));
}

void PhysicalLayerAsyncSerial::DoAsyncWrite(const boost::uint8_t* apBuffer, size_t aNumBytes)
{
	this->CaptureLog("TX:", apBuffer, aNumBytes);

	async_write(mPort, buffer(apBuffer, aNumBytes),
	            boost::bind(&PhysicalLayerAsyncSerial::OnWriteCallback,
	                        this,
	                        boost::asio::placeholders::error,
							apBuffer,
	                        aNumBytes));
}

}

/* vim: set ts=4 sw=4: */
