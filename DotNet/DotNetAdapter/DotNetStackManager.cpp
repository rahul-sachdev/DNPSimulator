// This is the main DLL file.

#include "stdafx.h"

#include "Conversions.h"
#include "DotNetStackManager.h"
#include <DNP3/MasterStackConfig.h>


namespace DNPDotNet {

	DotNetStackManager::DotNetStackManager() :
		pMgr(new apl::dnp::StackManager())
	{
		
	}

	void DotNetStackManager::AddTCPClient(System::String^ name, FilterLevelDN level, System::UInt64 retryMs, System::String^ address, System::UInt16 port)
	{
		std::string stdName = Conversions::convertString(name);
		std::string stdAddress = Conversions::convertString(address);
		boost::uint16_t stdPort = port;

		apl::PhysLayerSettings pls(Conversions::convertFilterLevel(level), retryMs);

		pMgr->AddTCPClient(stdName, pls, stdAddress, stdPort);
	}

	void DotNetStackManager::AddTCPServer(System::String^ name, FilterLevelDN level, System::UInt64 retryMs, System::String^ endpoint, System::UInt16 port)
	{
		std::string stdName = Conversions::convertString(name);
		std::string stdEndpoint = Conversions::convertString(endpoint);		
		boost::uint16_t stdPort = port;

		apl::PhysLayerSettings pls(Conversions::convertFilterLevel(level), retryMs);

		pMgr->AddTCPServer(stdName, pls, stdEndpoint, stdPort);
	}
		
	void DotNetStackManager::AddMaster(	System::String^ portName,
										System::String^ stackName,	                            
										FilterLevelDN level,
										IDataObserverDN^ observer)
	/*MasterStackConfigDN^ cfg)*/
	{
		std::string stdPortName = Conversions::convertString(portName);
		std::string stdStackName = Conversions::convertString(stackName);
		apl::FilterLevel stdLevel = Conversions::convertFilterLevel(level);

		apl::IDataObserver* pObserver = new MasterDataObserverAdapter(observer);
		apl::dnp::MasterStackConfig cfg; //defaults for now

		pMgr->AddMaster(stdPortName, stdStackName, stdLevel, pObserver, cfg);
	}
}