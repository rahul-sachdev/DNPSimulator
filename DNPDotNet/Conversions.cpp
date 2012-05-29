#include "stdafx.h"

#include <msclr\marshal_cppstd.h>
#include "Conversions.h"

namespace DNPDotNet {

	std::string Conversions::convertString(System::String^ s)
	{
		return msclr::interop::marshal_as<std::string>(s);
	}

	ControlCodeDN Conversions::convertControlCode(apl::ControlCode code)
	{
		switch(code)
		{
			case (apl::CC_NULL):
				return ControlCodeDN::CC_NULL;
			case (apl::CC_PULSE):
				return ControlCodeDN::CC_PULSE;
			case (apl::CC_LATCH_ON):
				return ControlCodeDN::CC_LATCH_ON;
			case (apl::CC_LATCH_OFF):
				return ControlCodeDN::CC_LATCH_OFF;
			case (apl::CC_PULSE_CLOSE):
				return ControlCodeDN::CC_PULSE_CLOSE;
			case (apl::CC_PULSE_TRIP):
				return ControlCodeDN::CC_PULSE_TRIP;			
			default:
				return ControlCodeDN::CC_UNDEFINED;
		}
	}

	apl::ControlCode Conversions::convertControlCode(ControlCodeDN code)
	{
		switch(code)
		{
			case (ControlCodeDN::CC_NULL):
				return apl::CC_NULL;
			case (ControlCodeDN::CC_PULSE):
				return apl::CC_PULSE;
			case (ControlCodeDN::CC_LATCH_ON):
				return apl::CC_LATCH_ON;
			case (ControlCodeDN::CC_LATCH_OFF):
				return apl::CC_LATCH_OFF;
			case (ControlCodeDN::CC_PULSE_CLOSE):
				return apl::CC_PULSE_CLOSE;
			case (ControlCodeDN::CC_PULSE_TRIP):
				return apl::CC_PULSE_TRIP;			
			default:
				return apl::CC_UNDEFINED;
		}
	}

	BinaryOutputDN^ Conversions::convertBO(const apl::BinaryOutput& bo)
	{
		return gcnew BinaryOutputDN(convertControlCode(bo.GetCode()), bo.mCount, bo.mOnTimeMS, bo.mOffTimeMS);
	}

	apl::BinaryOutput Conversions::convertBO(BinaryOutputDN^ bo)
	{
		return apl::BinaryOutput(convertControlCode(bo->code), bo->count, bo->onTime, bo->offTime);
	}

}
