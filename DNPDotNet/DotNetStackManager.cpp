// This is the main DLL file.

#include "stdafx.h"

#include "Conversions.h"
#include "DotNetStackManager.h"


namespace DNPDotNet {

	DotNetStackManager::DotNetStackManager() :
		pMgr(new apl::dnp::StackManager())
	{
		
	}

	void DotNetStackManager::AddTCPClient(System::String^ name, System::String^ address, int port)
	{
		std::string stdName = Conversions::convert(name);
		std::string stdAddress = Conversions::convert(address);
		boost::uint16_t stdPort = static_cast<boost::uint16_t>(port);

		apl::PhysLayerSettings pls;

		pMgr->AddTCPClient(stdName, pls, stdAddress, stdPort);
	}



}