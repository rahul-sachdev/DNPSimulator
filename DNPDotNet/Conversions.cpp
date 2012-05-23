#include "stdafx.h"

#include <msclr\marshal_cppstd.h>
#include "Conversions.h"

namespace DNPDotNet {

	std::string Conversions::convert(System::String^ s)
	{
		return msclr::interop::marshal_as<std::string>(s);
	}


}
