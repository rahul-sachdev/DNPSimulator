#ifndef __CONVERSIONS_H_
#define __CONVERSIONS_H_

#include <string>

#include "CommandTypes.h"
#include "APL\CommandTypes.h"

namespace DNPDotNet {

	class Conversions
	{
		public:
		static std::string convertString(System::String^ s);

		static ControlCodeDN convertControlCode(apl::ControlCode code);
		static apl::ControlCode Conversions::convertControlCode(ControlCodeDN code);
		static BinaryOutputDN^ convertBO(const apl::BinaryOutput& bo);
		static apl::BinaryOutput convertBO(BinaryOutputDN^ bo);
	};

}

#endif
