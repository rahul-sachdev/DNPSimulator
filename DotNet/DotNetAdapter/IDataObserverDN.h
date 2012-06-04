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
#ifndef __I_DATA_OBSERVER_DN_H_
#define __I_DATA_OBSERVER_DN_H_

using namespace System::Collections::ObjectModel;

#include <APL/DataInterfaces.h>
#include <vcclr.h>

using namespace DNP3::Interface;

namespace DNPDotNet
{	
	public interface class IDataObserverDN
	{
		void Start();
		void Update(Binary^ update, System::Int32 index);
		void Update(Analog^ update, System::Int32 index);
		void Update(Counter^ update, System::Int32 index);
		void Update(ControlStatus^ update, System::Int32 index);
		void Update(SetpointStatus^ update, System::Int32 index);
		void End();
	};

	public class MasterDataObserverAdapter : public apl::IDataObserver
	{
		public:

		MasterDataObserverAdapter(IDataObserverDN^ proxy);

		private:

		gcroot<IDataObserverDN^> proxy;

		protected:

		void _Start();
		void _Update(const apl::Binary& arPoint, size_t aIndex);
		void _Update(const apl::Analog& arPoint, size_t aIndex);
		void _Update(const apl::Counter& arPoint, size_t aIndex);
		void _Update(const apl::ControlStatus& arPoint, size_t aIndex);
		void _Update(const apl::SetpointStatus& arPoint, size_t aIndex);
		void _End();
	};
}

#endif
