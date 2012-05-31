#include "stdafx.h"

#include "CommandTypesDN.h"
#include <boost\thread\thread.hpp>

namespace DNPDotNet
{

	BinaryOutputDN::BinaryOutputDN(ControlCodeDN code, System::Byte count, System::UInt16 onTime, System::UInt16 offTime) : 
		code(code), count(count), onTime(onTime), offTime(offTime)
	{
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}
		
	SetpointDN::SetpointDN(System::Int16 value) : value(value), encodingType(SetpointEncodingTypeDN::SPET_INT16)
	{}
	
	SetpointDN::SetpointDN(System::Int32 value) : value(value), encodingType(SetpointEncodingTypeDN::SPET_INT32)
	{}

	SetpointDN::SetpointDN(System::Double value) : value(value), encodingType(SetpointEncodingTypeDN::SPET_DOUBLE)
	{}

	SetpointDN::SetpointDN(System::Single value) : value(value), encodingType(SetpointEncodingTypeDN::SPET_FLOAT)
	{}

	
}

