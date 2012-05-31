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
#ifndef __CONFIG_OBJECTS_H_
#define __CONFIG_OBJECTS_H_

using namespace System::Collections::Generic;

namespace DNPDotNet
{
	public ref class LinkConfigDN
	{
		public:
		LinkConfigDN(	System::Boolean isMaster, 
						System::Boolean useConfirms, 
						System::UInt32 numRetry, 
						System::UInt16 localAddr, 
						System::Boolean remoteAddr, 
						System::UInt64 timeout) :
							isMaster(isMaster),
							useConfirms(useConfirms),
							numRetry(numRetry),
							localAddr(localAddr),
							remoteAddr(remoteAddr),
							timeout(timeout)
		{}


		LinkConfigDN(System::Boolean aIsMaster, System::Boolean aUseConfirms) :
			isMaster(isMaster),
			useConfirms(useConfirms),
			numRetry(0),
			localAddr(isMaster ? 1 : 1024),
			remoteAddr(isMaster ? 1024 : 1),
			timeout(1000)
		{}


		// The master/slave bit set on all messages
		System::Boolean isMaster;

		// If true, the link layer will send data requesting confirmation
		System::Boolean useConfirms;

		// The number of retry attempts the link will attempt after the initial try
		System::UInt32 numRetry;

		// dnp3 address of the local device
		System::UInt16 localAddr;

		// dnp3 address of the remote device
		System::Boolean remoteAddr;

		// the response timeout in milliseconds for confirmed requests
		System::UInt64 timeout;
	};

	public ref class AppConfigDN
	{
		public:
		AppConfigDN() : rspTimeout(5000), numRetry(0), fragSize(2048) {}

		AppConfigDN(System::Int64 rspTimeout, System::Int32 aNumRetry, System::Int32 fragSize) :
			rspTimeout(rspTimeout),
			numRetry(numRetry),
			fragSize(fragSize)
		{}

		// The response/confirm timeout in millisec
		System::Int64 rspTimeout;

		// Number of retries performed for applicable frames
		System::Int32 numRetry;

		// The maximum size of received application layer fragments
		System::Int32 fragSize;
	};
	
	public ref class ExceptionScanDN {
		public:
			ExceptionScanDN(System::Int32 classMask, System::Int64 scanRateMs) :
				classMask(classMask), scanRateMs(scanRateMs)
				{}

		System::Int32 classMask;
		System::Int64 scanRateMs;

	};
	
	public enum class PointClassDN {
		PC_CLASS_0 = 0x01,
		PC_CLASS_1 = 0x02,
		PC_CLASS_2 = 0x04,
		PC_CLASS_3 = 0x08,
		PC_ALL_EVENTS = PC_CLASS_1 | PC_CLASS_2 | PC_CLASS_3		
	};

	/// Configuration information for the dnp3 master
	public ref class MasterConfigDN {
		public:
		// Default constructor
		MasterConfigDN() :
			fragSize(2048),				
			allowTimeSync(true),
			doUnsolOnStartup(false),
			enableUnsol(true),
			unsolClassMask(static_cast<System::Int32>(PointClassDN::PC_ALL_EVENTS)),
			integrityRate(5000),
			taskRetryRate(5000),
			scans(gcnew List<ExceptionScanDN^>()) 
		{}

		// 
		void AddExceptionScan(System::Int32 classMask, System::Int64 period) {
			scans.Add(gcnew ExceptionScanDN(classMask, period));
			
		}

		// Maximum fragment size to use for requests
		System::Int32 fragSize;	

		// If true, the master will do time syncs when it sees the time IIN bit from the slave
		System::Boolean allowTimeSync;

		// If true, the master will enable/disable unsol on startup
		System::Boolean doUnsolOnStartup;

		// If DoUnsolOnStartup == true, the master will use this bit to decide wether to enable (true) or disable (false)
		System::Boolean enableUnsol;

		//	Bitwise mask used determine which classes are enabled/disabled for unsol
		System::Int32 unsolClassMask;

		// Period for integrity scans (class 0), -1 for non periodic
		System::Int64 integrityRate;

		// Time delay between task retries
		System::Int64 taskRetryRate;

		// vector that holds exception scans
		List<ExceptionScanDN^> scans;
	};

	public ref class MasterStackConfigDN {
	
		public:

		MasterStackConfigDN() :
			link(true, false)
		{}

		MasterConfigDN master;
		AppConfigDN app;
		LinkConfigDN link;
	};

}

/* vim: set ts=4 sw=4: */

#endif
