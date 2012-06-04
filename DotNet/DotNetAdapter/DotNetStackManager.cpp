// This is the main DLL file.

#include "stdafx.h"

#include "DotNetStackManager.h"

#include "Conversions.h"
#include "MasterDataObserverAdapter.h"
#include "CommandAcceptorAdapter.h"

#include <DNP3/MasterStackConfig.h>
#include <DNP3/StackManager.h>

namespace DNPDotNet {

	DotNetStackManager::DotNetStackManager() :
		pMgr(new apl::dnp::StackManager())
	{
		
	}

	void DotNetStackManager::AddTCPClient(System::String^ name, FilterLevel level, System::UInt64 retryMs, System::String^ address, System::UInt16 port)
	{
		std::string stdName = Conversions::convertString(name);
		std::string stdAddress = Conversions::convertString(address);
		boost::uint16_t stdPort = port;

		apl::PhysLayerSettings pls(Conversions::convertFilterLevel(level), retryMs);

		pMgr->AddTCPClient(stdName, pls, stdAddress, stdPort);
	}

	void DotNetStackManager::AddTCPServer(System::String^ name, FilterLevel level, System::UInt64 retryMs, System::String^ endpoint, System::UInt16 port)
	{
		std::string stdName = Conversions::convertString(name);
		std::string stdEndpoint = Conversions::convertString(endpoint);		
		boost::uint16_t stdPort = port;

		apl::PhysLayerSettings pls(Conversions::convertFilterLevel(level), retryMs);

		pMgr->AddTCPServer(stdName, pls, stdEndpoint, stdPort);
	}
		
	ICommandAcceptor^ DotNetStackManager::AddMaster(	System::String^ portName,
														System::String^ stackName,	                            
														FilterLevel level,
														DNP3::Interface::IDataObserver^ observer,
														DNP3::Interface::MasterStackConfig^ config)	
	{
		std::string stdPortName = Conversions::convertString(portName);
		std::string stdStackName = Conversions::convertString(stackName);
		apl::FilterLevel stdLevel = Conversions::convertFilterLevel(level);

		apl::IDataObserver* pObserver = new MasterDataObserverAdapter(observer);
		apl::dnp::MasterStackConfig cfg = Conversions::convertConfig(config);

		apl::ICommandAcceptor* pCmdAcceptor = pMgr->AddMaster(stdPortName, stdStackName, stdLevel, pObserver, cfg);
		ICommandAcceptor^ ca = gcnew CommandAcceptorAdapter(pCmdAcceptor);
		return ca;
	}
}