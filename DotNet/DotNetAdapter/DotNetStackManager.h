// DNPDotNet.h

#pragma once

using namespace System;
#include <DNP3/StackManager.h>
#include <memory>
#include "LogTypesDN.h"
#include "IDataObserverDN.h"

using namespace DNP3::Interface;

namespace DNPDotNet {

	public ref class DotNetStackManager
	{
		public:
			DotNetStackManager();

			
			void AddTCPClient(System::String^ name, FilterLevelDN level, System::UInt64 retryMs, System::String^ address, System::UInt16 port);					
			void AddTCPServer(System::String^ name, FilterLevelDN level, System::UInt64 retryMs, System::String^ endpoint, System::UInt16 port);

			//void AddSerial(const std::string& arName, FilterLevelDN level, System::UInt64 retryMs, SerialSettings aSerial);
						
			void AddMaster(	System::String^ portName,
							System::String^ stackName,	                            
	                        FilterLevelDN level,
	                        IDataObserver^ publisher);
	                        //MasterStackConfigDN^ cfg)

		private:
			apl::dnp::StackManager* pMgr;
	};


}
