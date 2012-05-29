//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  Green Enery Corp licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
#ifndef __COMMAND_TYPES_DN_H_
#define __COMMAND_TYPES_DN_H_

namespace DNPDotNet
{

public enum class CommandModesDN {
	CM_SBO_ONLY,
	CM_DO_ONLY,	
	CM_SBO_OR_DO
};

public enum class CommandTypesDN {
	CT_BINARY_OUTPUT,
	CT_SETPOINT,
	CT_NONE
};


public enum class CommandStatusDN {
	CS_SUCCESS,			
	CS_TIMEOUT,			
	CS_NO_SELECT,		
	CS_FORMAT_ERROR,	
	CS_NOT_SUPPORTED,	
	CS_ALREADY_ACTIVE,	
	CS_HARDWARE_ERROR,	
	CS_LOCAL,			
	CS_TOO_MANY_OPS,	
	CS_NOT_AUTHORIZED,
	CS_UNDEFINED
};

public enum class ControlCodeDN {
	CC_NULL,
	CC_PULSE,
	CC_LATCH_ON,
	CC_LATCH_OFF,
	CC_PULSE_CLOSE,
	CC_PULSE_TRIP,
	CC_UNDEFINED
};

public ref class BinaryOutputDN
{
	public:	

	BinaryOutputDN(ControlCodeDN code, System::Byte count, System::UInt16 onTime, System::UInt16 offTime);	

	initonly ControlCodeDN code;
	initonly System::Byte count;	
	initonly System::UInt16 onTime;
	initonly System::UInt16 offTime;
};

/*
enum SetpointEncodingType {
	SPET_INT16,			//!< 16bit floating point (dnp Object41var1)
	SPET_INT32,			//!< 32bit signed integer (dnp Object41var2)
	SPET_FLOAT,			//!< 32bit floating point (dnp Object41var3)
	SPET_DOUBLE,		//!< 64bit floating point (dnp Object41var4)
	SPET_AUTO_INT,		//!< automatically choose smallest valid int type
	SPET_AUTO_DOUBLE,	//!< automatically choose smallest valid double type
	SPET_UNSET,			//!< means no type has been guessed or set yet
};

class Setpoint : public CommandRequest
{
public:
	
	Setpoint(boost::int16_t aValue);
	
	Setpoint(boost::int32_t aValue);

	
	Setpoint(double aValue);

	
	Setpoint();

	std::string ToString() const;

	bool operator==(const Setpoint& arRHS) const {
		return fabs(mValue - arRHS.mValue) < 1E-6;
	}

	static const CommandTypes EnumType = CT_SETPOINT;

	boost::int32_t GetIntValue() const {
		return static_cast<boost::int32_t>(GetValue());
	}
	double GetValue() const;

	void SetValue(double aValue);
	void SetValue(boost::int32_t aValue);

	SetpointEncodingType GetOptimalEncodingType() const;

	SetpointEncodingType GetEncodingType() const {
		return mEncodingType;
	}
	void SetEncodingType(SetpointEncodingType aEncodingType) {
		mEncodingType = aEncodingType;
	}

private:
	double mValue;

	SetpointEncodingType mEncodingType;

};

class CommandResponse
{
public:
	CommandResponse() {}
	CommandResponse(CommandStatus aStatus) : mResult(aStatus) {}

	CommandStatus mResult;
};
*/

}

/* vim: set ts=4 sw=4: */

#endif
