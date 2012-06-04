#include "stdafx.h"

#include <msclr\marshal_cppstd.h>
#include "Conversions.h"

namespace DNPDotNet {

	System::DateTime Conversions::convertTime(apl::millis_t time)
	{
		// each 'tick' represents 100 nanoseconds
		return System::DateTime(10000*time);
	}

	std::string Conversions::convertString(System::String^ s)
	{
		return msclr::interop::marshal_as<std::string>(s);
	}

	apl::FilterLevel Conversions::convertFilterLevel(FilterLevel level)
	{
		switch(level)
		{
			case (FilterLevel::LEV_EVENT):
				return apl::LEV_EVENT;
			case (FilterLevel::LEV_ERROR):
				return apl::LEV_ERROR;
			case (FilterLevel::LEV_WARNING):
				return apl::LEV_WARNING;
			case (FilterLevel::LEV_INFO):
				return apl::LEV_INFO;
			case (FilterLevel::LEV_INTERPRET):
				return apl::LEV_INTERPRET;
			case (FilterLevel::LEV_COMM):
				return apl::LEV_COMM;
			case (FilterLevel::LEV_DEBUG):
				return apl::LEV_DEBUG;
			default:
				return apl::LEV_DEBUG;
		}
	}

	CommandStatus Conversions::convertCommandStatus(apl::CommandStatus status)
	{
		switch(status)
		{
			case(apl::CS_SUCCESS):
				return CommandStatus::CS_SUCCESS;
			case(apl::CS_TIMEOUT):
				return CommandStatus::CS_TIMEOUT;
			case(apl::CS_NO_SELECT):
				return CommandStatus::CS_NO_SELECT;
			case(apl::CS_FORMAT_ERROR):
				return CommandStatus::CS_FORMAT_ERROR;
			case(apl::CS_NOT_SUPPORTED):
				return CommandStatus::CS_NOT_SUPPORTED;
			case(apl::CS_ALREADY_ACTIVE):
				return CommandStatus::CS_ALREADY_ACTIVE;
			case(apl::CS_HARDWARE_ERROR):
				return CommandStatus::CS_HARDWARE_ERROR;
			case(apl::CS_LOCAL):
				return CommandStatus::CS_LOCAL;
			case(apl::CS_TOO_MANY_OPS):
				return CommandStatus::CS_TOO_MANY_OPS;
			case(apl::CS_NOT_AUTHORIZED):
				return CommandStatus::CS_NOT_AUTHORIZED;

			default:
				return CommandStatus::CS_UNDEFINED;
		}
	}

	ControlCode Conversions::convertControlCode(apl::ControlCode code)
	{
		switch(code)
		{
			case (apl::CC_NULL):
				return ControlCode::CC_NULL;
			case (apl::CC_PULSE):
				return ControlCode::CC_PULSE;
			case (apl::CC_LATCH_ON):
				return ControlCode::CC_LATCH_ON;
			case (apl::CC_LATCH_OFF):
				return ControlCode::CC_LATCH_OFF;
			case (apl::CC_PULSE_CLOSE):
				return ControlCode::CC_PULSE_CLOSE;
			case (apl::CC_PULSE_TRIP):
				return ControlCode::CC_PULSE_TRIP;			
			default:
				return ControlCode::CC_UNDEFINED;
		}
	}

	apl::ControlCode Conversions::convertControlCode(ControlCode code)
	{
		switch(code)
		{
			case (ControlCode::CC_NULL):
				return apl::CC_NULL;
			case (ControlCode::CC_PULSE):
				return apl::CC_PULSE;
			case (ControlCode::CC_LATCH_ON):
				return apl::CC_LATCH_ON;
			case (ControlCode::CC_LATCH_OFF):
				return apl::CC_LATCH_OFF;
			case (ControlCode::CC_PULSE_CLOSE):
				return apl::CC_PULSE_CLOSE;
			case (ControlCode::CC_PULSE_TRIP):
				return apl::CC_PULSE_TRIP;			
			default:
				return apl::CC_UNDEFINED;
		}
	}

	BinaryOutput^ Conversions::convertBO(const apl::BinaryOutput& bo)
	{
		return gcnew BinaryOutput(convertControlCode(bo.GetCode()), bo.mCount, bo.mOnTimeMS, bo.mOffTimeMS);
	}

	apl::BinaryOutput Conversions::convertBO(BinaryOutput^ bo)
	{
		return apl::BinaryOutput(convertControlCode(bo->code), bo->count, bo->onTime, bo->offTime);
	}


	SetpointEncodingType Conversions::convertSetpointEncoding(apl::SetpointEncodingType encoding)
	{
		switch(encoding) {
			case (apl::SPET_INT16):
				return SetpointEncodingType::SPET_INT16;
			case (apl::SPET_INT32):
				return SetpointEncodingType::SPET_INT32;
			case (apl::SPET_FLOAT):
				return SetpointEncodingType::SPET_FLOAT;
			case (apl::SPET_DOUBLE):
				return SetpointEncodingType::SPET_DOUBLE;
			case (apl::SPET_AUTO_INT):
				return SetpointEncodingType::SPET_AUTO_INT;
			case (apl::SPET_AUTO_DOUBLE):
				return SetpointEncodingType::SPET_AUTO_DOUBLE;
			default:
				return SetpointEncodingType::SPET_UNSET;
		}
	}

	apl::SetpointEncodingType Conversions::convertSetpointEncoding(SetpointEncodingType encoding)
	{
		switch(encoding) {
			case (SetpointEncodingType::SPET_INT16):
				return apl::SPET_INT16;
			case (SetpointEncodingType::SPET_INT32):
				return apl::SPET_INT32;
			case (SetpointEncodingType::SPET_FLOAT):
				return apl::SPET_FLOAT;
			case (SetpointEncodingType::SPET_DOUBLE):
				return apl::SPET_DOUBLE;
			case (SetpointEncodingType::SPET_AUTO_INT):
				return apl::SPET_AUTO_INT;
			case (SetpointEncodingType::SPET_AUTO_DOUBLE):
				return apl::SPET_AUTO_DOUBLE;
			default:
				return apl::SPET_UNSET;
		}
	}

	apl::Setpoint Conversions::convertSP(Setpoint^ sp)
	{
		apl::Setpoint ret(sp->value);
		ret.SetEncodingType(convertSetpointEncoding(sp->encodingType));
		return ret;
	}

	Binary^ Conversions::convertMeas(apl::Binary meas)
	{
		return gcnew Binary(meas.GetValue(), meas.GetQuality(), convertTime(meas.GetTime()));
	}

	Analog^ Conversions::convertMeas(apl::Analog meas)
	{
		return gcnew Analog(meas.GetValue(), meas.GetQuality(), convertTime(meas.GetTime()));
	}

	Counter^ Conversions::convertMeas(apl::Counter meas)
	{
		return gcnew Counter(meas.GetValue(), meas.GetQuality(), convertTime(meas.GetTime()));
	}

	SetpointStatus^ Conversions::convertMeas(apl::SetpointStatus meas)
	{
		return gcnew SetpointStatus(meas.GetValue(), meas.GetQuality(), convertTime(meas.GetTime()));
	}

	ControlStatus^ Conversions::convertMeas(apl::ControlStatus meas)
	{
		return gcnew ControlStatus(meas.GetValue(), meas.GetQuality(), convertTime(meas.GetTime()));
	}
}
