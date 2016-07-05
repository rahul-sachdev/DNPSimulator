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

#include "MasterDemo.h"

using namespace std;

namespace apl { namespace dnp {

MasterDemoBase::MasterDemoBase(Logger* apLogger)
	: IOService()
	, IOServiceThread(apLogger, this->Get())
	, mTimerSource(this->Get())
	, mpInfiniteTimer(mTimerSource.StartInfinite())
	, mTestCounter(min_temp)
{
	/*
	 * Create a notifier that will be called whenever data updates
	 * are received.
	 */
	INotifier* pNotifier =
		mPostSource.Get(
			boost::bind(
				&MasterDemoBase::OnDataUpdate,
				this
			),
			&mTimerSource
		);

	/*
	 * Register the INotifier with the QueueingFDO instance.
	 */
	mFDO.AddObserver(pNotifier);
}

void MasterDemoBase::Shutdown()
{
	// this is the only outstanding event, so this will cause the
	// io_service thread to exit cleanly
	mpInfiniteTimer->Cancel();
}

void MasterDemoBase::OnDataUpdate()
{
	// Obtain a lock on the QueueingFDO
	Transaction tr(&mFDO);
	// Print the contents of the update queue
	size_t num_updates = mFDO.updates.size();
	string data;
	string analogVal = "Analog";
	string delimiter = ">";
	string delimiter2 = "{O";
	size_t delimiterVal,delimiter2Val;
	string currTempString;
	string SlimTemp;
	size_t currTempint;
	if ( num_updates > 0 ) {
		LOG_BLOCK(LEV_EVENT,
				"Received " << num_updates
				<< " updates");
		do {
	//FETCH ANALOG VALUE from update
	LOG_BLOCK(LEV_INFO, mFDO.updates.front());
	data = mFDO.updates.front();
	size_t analogFound = data.find(analogVal);
	//cout << "ANALOGFOUND =" << analogFound << endl;
	if(analogFound == 0 ) {// std::string::npos) {
		 delimiterVal  = data.find(delimiter);
		 delimiter2Val = data.find_last_of(delimiter2);
		 
	//	cout << "> = " << delimiterVal << endl;
	//	cout << "{ = " << delimiter2Val << endl;
		 currTempString = data.substr(delimiterVal + 1,delimiter2Val-delimiterVal-2);
	//	cout << "STRING = [" << currTempString << "]" << endl ;
		currTempint = stoi(currTempString);
	//	cout << "INT = [" << currTempint << "]" << endl;
	}
	//FETCH INDEX OF ANALOG
	//IF FOUND FETCH INDEX OF - and add 3 to it
	//FETCH INDEX OF { 
	//FETCH VALUE between those and trim
	
		
	LOG_BLOCK(LEV_INFO, mFDO.updates.front());
			mFDO.updates.pop_front();
		} while ( !mFDO.updates.empty() );
	}

	if ( num_updates >= 1 ) {
		// Increment the values on the Setpoints (Analog Outputs)
		typedef PointMap<SetpointStatus>::Type SetpointMap;
		typedef SetpointMap::iterator SetpointMapIter;
		int index = 0;
		SetpointMapIter iter = mFDO.mSetpointStatusMap.begin();

		for (; iter != mFDO.mSetpointStatusMap.end(); ++iter, ++index)
		{
			const int16_t value = ++mTestCounter;
			const int16_t tempVal = currTempint;
			if ( mTestCounter < 0 )
				mTestCounter = 0;

			LOG_BLOCK(LEV_EVENT,"Minimum Temperature = [0], Maximum Temperatire = [100], Current Temperature = ["
					<< currTempint << "]");
	//		masterfile << currTempint << endl;
			//Setpoint sp(value%max_temp);
			Setpoint sp(tempVal);
			mpCommandAcceptor->AcceptCommand(
				sp,    // Setpoint instance
				index, // point index
				index, // sequence value
				this   // IResponseAcceptor*
			);
		}
//		LOG_BLOCK(LEV_INFO,
//				"Sent " << index
//				<< " updates");
	}
//	masterfile.close();
}



/*
void MasterDemoBase::OnDataUpdateBinary()
{
	// Obtain a lock on the QueueingFDO
	Transaction tr(&mFDO);

	// Print the contents of the update queue
	size_t num_updates = mFDO.updates.size();
	if ( num_updates > 0 ) {
		LOG_BLOCK(LEV_INFO,
				"Received " << num_updates
				<< " updates");
		do {
			LOG_BLOCK(LEV_INFO, mFDO.updates.front());
			mFDO.updates.pop_front();
		} while ( !mFDO.updates.empty() );
	}

	if ( num_updates > 1 ) {
		// Increment the values on the Setpoints (Analog Outputs)
		typedef PointMap<Binary>::Type BinaryMap;
		typedef BinaryMap::iterator BinaryMapIter;
		int index = 0;
		int mTestCounter = 0;
		BinaryMapIter iter = mFDO.mBinaryMap.begin();
		for (; iter != mFDO.mBinaryMap.end(); ++iter, ++index)
		{
			const int16_t value = ++mTestCounter;
			if ( mTestCounter < 0 )
				mTestCounter = 0;

			LOG_BLOCK(LEV_DEBUG, "Assigning new value "
					<< value
					<< " to index "
					<< index);
			BinaryOutput bo;
			if (value%2 == 0) {
				bo = CC_LATCH_ON;
			} else {
				bo = CC_LATCH_OFF;
			}
			mpCommandAcceptor->AcceptCommand(
				bo,    // Setpoint instance
				index, // point index
				value, // sequence value
				this   // IResponseAcceptor*
			);
		}
		LOG_BLOCK(LEV_INFO,
				"Sent " << index
				<< " updates");
	}
}


*/


void MasterDemoBase::AcceptResponse(const CommandResponse& aResponse,
		int aSequence)
{
	LOG_BLOCK(LEV_DEBUG, "Response received for update to index "
			<< aSequence << ": "
			<< ToString(aResponse.mResult));
}

MasterDemoApp::MasterDemoApp(Logger* apLogger)
	: MasterDemoBase(apLogger)
{}

void MasterDemoApp::OnStateChange(StackStates aState)
{
	LOG_BLOCK(LEV_INFO, "Communications link state change: "
			<< ConvertStackStateToString(aState));
}

}} // end namespaces apl and apl::dnp

