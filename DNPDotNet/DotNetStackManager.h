// DNPDotNet.h

#pragma once

using namespace System;
#include <DNP3/StackManager.h>
#include <memory>

namespace DNPDotNet {

	public ref class DotNetStackManager
	{
		public:
			DotNetStackManager();

			// TODO - add physical layer settings
			void AddTCPClient(System::String^ name, System::String^ address, int port);

			/* Physical layer functions			
			void AddTCPServer(const std::string& arName, PhysLayerSettings  aPhys, const std::string& arEndpoint, boost::uint16_t aPort);
			void AddSerial(const std::string& arName, PhysLayerSettings aPhys, SerialSettings aSerial);
			*/

			/*
			ICommandAcceptor* AddMaster(const std::string& arPortName,
	                            const std::string& arStackName,
	                            FilterLevel aLevel,
	                            IDataObserver* apPublisher,
	                            const MasterStackConfig& arCfg);
			*/

		private:
			apl::dnp::StackManager* pMgr;
	};


}
