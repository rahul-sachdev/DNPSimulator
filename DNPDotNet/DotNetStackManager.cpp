// This is the main DLL file.

#include "stdafx.h"

#include "Conversions.h"
#include "DotNetStackManager.h"


namespace DNPDotNet {

	DotNetStackManager::DotNetStackManager() :
		pMgr(new apl::dnp::StackManager())
	{
		
	}

	void DotNetStackManager::AddTCPClient(System::String^ name, System::String^ address, System::UInt16^ port)
	{
		std::string stdName = Conversions::convertString(name);
		std::string stdAddress = Conversions::convertString(address);
		boost::uint16_t stdPort = static_cast<boost::uint16_t>(port);

		apl::PhysLayerSettings pls;

		pMgr->AddTCPClient(stdName, pls, stdAddress, stdPort);
	}



}