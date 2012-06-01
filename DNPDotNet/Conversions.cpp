#include "stdafx.h"

#include <msclr\marshal_cppstd.h>
#include "Conversions.h"

namespace DNPDotNet {

	std::string Conversions::convertString(System::String^ s)
	{
		return msclr::interop::marshal_as<std::string>(s);
	}

	apl::FilterLevel Conversions::convertFilterLevel(FilterLevelDN level)
	{
		switch(level)
		{
			case (FilterLevelDN::LEV_EVENT):
				return apl::LEV_EVENT;
			case (FilterLevelDN::LEV_ERROR):
				return apl::LEV_ERROR;
			case (FilterLevelDN::LEV_WARNING):
				return apl::LEV_WARNING;
			case (FilterLevelDN::LEV_INFO):
				return apl::LEV_INFO;
			case (FilterLevelDN::LEV_INTERPRET):
				return apl::LEV_INTERPRET;
			case (FilterLevelDN::LEV_COMM):
				return apl::LEV_COMM;
			case (FilterLevelDN::LEV_DEBUG):
				return apl::LEV_DEBUG;
			default:
				return apl::LEV_DEBUG;
		}
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


	SetpointEncodingTypeDN Conversions::convertSetpointEncoding(apl::SetpointEncodingType encoding)
	{
		switch(encoding) {
			case (apl::SPET_INT16):
				return SetpointEncodingTypeDN::SPET_INT16;
			case (apl::SPET_INT32):
				return SetpointEncodingTypeDN::SPET_INT32;
			case (apl::SPET_FLOAT):
				return SetpointEncodingTypeDN::SPET_FLOAT;
			case (apl::SPET_DOUBLE):
				return SetpointEncodingTypeDN::SPET_DOUBLE;
			case (apl::SPET_AUTO_INT):
				return SetpointEncodingTypeDN::SPET_AUTO_INT;
			case (apl::SPET_AUTO_DOUBLE):
				return SetpointEncodingTypeDN::SPET_AUTO_DOUBLE;
			default:
				return SetpointEncodingTypeDN::SPET_UNSET;
		}
	}

	apl::SetpointEncodingType Conversions::convertSetpointEncoding(SetpointEncodingTypeDN encoding)
	{
		switch(encoding) {
			case (SetpointEncodingTypeDN::SPET_INT16):
				return apl::SPET_INT16;
			case (SetpointEncodingTypeDN::SPET_INT32):
				return apl::SPET_INT32;
			case (SetpointEncodingTypeDN::SPET_FLOAT):
				return apl::SPET_FLOAT;
			case (SetpointEncodingTypeDN::SPET_DOUBLE):
				return apl::SPET_DOUBLE;
			case (SetpointEncodingTypeDN::SPET_AUTO_INT):
				return apl::SPET_AUTO_INT;
			case (SetpointEncodingTypeDN::SPET_AUTO_DOUBLE):
				return apl::SPET_AUTO_DOUBLE;
			default:
				return apl::SPET_UNSET;
		}
	}

	BinaryDN^ Conversions::convertMeas(apl::Binary meas)
	{
		return gcnew BinaryDN(meas.GetValue(), meas.GetQuality());
	}

	AnalogDN^ Conversions::convertMeas(apl::Analog meas)
	{
		return gcnew AnalogDN(meas.GetValue(), meas.GetQuality());
	}

	CounterDN^ Conversions::convertMeas(apl::Counter meas)
	{
		return gcnew CounterDN(meas.GetValue(), meas.GetQuality());
	}

	SetpointStatusDN^ Conversions::convertMeas(apl::SetpointStatus meas)
	{
		return gcnew SetpointStatusDN(meas.GetValue(), meas.GetQuality());
	}

	ControlStatusDN^ Conversions::convertMeas(apl::ControlStatus meas)
	{
		return gcnew ControlStatusDN(meas.GetValue(), meas.GetQuality());
	}
}
