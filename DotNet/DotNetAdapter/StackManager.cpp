// This is the main DLL file.

#include "stdafx.h"

#include "StackManager.h"

#include "Conversions.h"
#include "MasterDataObserverAdapter.h"
#include "CommandAcceptorAdapter.h"

#include "SlaveCommandAcceptorAdapter.h"
#include "SlaveDataObserverAdapter.h"

#include "LogAdapter.h"

#include <DNP3/MasterStackConfig.h>
#include <DNP3/SlaveStackConfig.h>
#include <DNP3/StackManager.h>

namespace DNPDotNet {

	StackManager::StackManager() :
		pMgr(new apl::dnp::StackManager())
	{
		
	}

	void StackManager::AddTCPClient(System::String^ name, FilterLevel level, System::UInt64 retryMs, System::String^ address, System::UInt16 port)
	{
		std::string stdName = Conversions::convertString(name);
		std::string stdAddress = Conversions::convertString(address);
		boost::uint16_t stdPort = port;

		apl::PhysLayerSettings pls(Conversions::convertFilterLevel(level), retryMs);

		pMgr->AddTCPClient(stdName, pls, stdAddress, stdPort);
	}

	void StackManager::AddTCPServer(System::String^ name, FilterLevel level, System::UInt64 retryMs, System::String^ endpoint, System::UInt16 port)
	{
		std::string stdName = Conversions::convertString(name);
		std::string stdEndpoint = Conversions::convertString(endpoint);		
		boost::uint16_t stdPort = port;

		apl::PhysLayerSettings pls(Conversions::convertFilterLevel(level), retryMs);

		pMgr->AddTCPServer(stdName, pls, stdEndpoint, stdPort);
	}
		
	ICommandAcceptor^ StackManager::AddMaster(	System::String^ portName,
												System::String^ stackName,	                            
												FilterLevel level,
												DNP3::Interface::IDataObserver^ observer,
												DNP3::Interface::MasterStackConfig^ config)	
	{
		std::string stdPortName = Conversions::convertString(portName);
		std::string stdStackName = Conversions::convertString(stackName);
		apl::FilterLevel stdLevel = Conversions::convertFilterLevel(level);

		MasterDataObserverAdapterWrapper^ wrapper = gcnew MasterDataObserverAdapterWrapper(observer);		
		apl::dnp::MasterStackConfig cfg = Conversions::convertConfig(config);

		apl::ICommandAcceptor* pCmdAcceptor = pMgr->AddMaster(stdPortName, stdStackName, stdLevel, wrapper->GetDataObserver(), cfg);
		ICommandAcceptor^ ca = gcnew CommandAcceptorAdapter(pCmdAcceptor);
		return ca;
	}

	
	IDataObserver^	StackManager::AddSlave(	System::String^ portName,
									System::String^ stackName,
									FilterLevel level,
									ICommandAcceptor^ cmdAcceptor )
	{
		std::string stdPortName = Conversions::convertString(portName);
		std::string stdStackName = Conversions::convertString(stackName);
		apl::FilterLevel stdLevel = Conversions::convertFilterLevel(level);

		SlaveCommandAcceptorAdapterWrapper^ wrapper = gcnew SlaveCommandAcceptorAdapterWrapper(cmdAcceptor);

		apl::dnp::SlaveStackConfig cfg; //TODO -- replace defaults
		cfg.device = apl::dnp::DeviceTemplate(10,10,10,10,10,10,10);

		apl::IDataObserver* pDataObs = pMgr->AddSlave(stdPortName, stdStackName, stdLevel, wrapper->GetCommandAcceptor(), cfg);
		return gcnew SlaveDataObserverAdapter(pDataObs);
	}

	void StackManager::AddLogHandler(ILogHandler^ logHandler)
	{
		LogAdapterWrapper^ wrapper = gcnew LogAdapterWrapper(logHandler);
		pMgr->AddLogHook(wrapper->GetLogAdapter());
	}
}