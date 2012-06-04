#ifndef __CONVERSIONS_H_
#define __CONVERSIONS_H_

#include <string>

#include <APL/CommandTypes.h>
#include <APL/LogTypes.h>
#include <APL/DataTypes.h>

using namespace DNP3::Interface;

namespace DNPDotNet {

	class Conversions
	{
		public:

		// Convert timestamps
		static System::DateTime convertTime(apl::millis_t time);
		
		// Convert a .NET string to a C++ string
		static std::string convertString(System::String^ s);

		// Convert the log filter enumeration
		static apl::FilterLevel convertFilterLevel(FilterLevel level);

		// Convert the command status enumeration
		static CommandStatus convertCommandStatus(apl::CommandStatus status);

		//functions for converting binary outputs		

		static ControlCode convertControlCode(apl::ControlCode code);
		static apl::ControlCode Conversions::convertControlCode(ControlCode code);
		static BinaryOutput^ convertBO(const apl::BinaryOutput& bo);
		static apl::BinaryOutput convertBO(BinaryOutput^ bo);

		//functions for converting setpoints

		static SetpointEncodingType convertSetpointEncoding(apl::SetpointEncodingType encoding);
		static apl::SetpointEncodingType convertSetpointEncoding(SetpointEncodingType encoding);
		static apl::Setpoint convertSP(Setpoint^ sp);

		//functions for converting Measurement types
		static Binary^ convertMeas(apl::Binary meas);
		static Analog^ convertMeas(apl::Analog meas);
		static Counter^ convertMeas(apl::Counter meas);
		static SetpointStatus^ convertMeas(apl::SetpointStatus meas);
		static ControlStatus^ convertMeas(apl::ControlStatus meas);
	};

}

#endif
