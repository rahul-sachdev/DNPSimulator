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

#include "config.h"
#include "StackManager.h"

#include <DNP3/AsyncStackManager.h>
#include <APL/Log.h>

namespace apl
{
namespace dnp
{

StackManager::StackManager() :
	mpLog(new EventLog()),
	mpImpl(new AsyncStackManager(mpLog->GetLogger(LEV_WARNING, "dnp")))
{

}

void StackManager::AddLogHook(ILogBase* apHook)
{
	mpLog->AddLogSubscriber(apHook);
}

StackManager::~StackManager()
{
	delete mpImpl;
	delete mpLog;
}

#if ENABLE_TCP_CLIENT
void StackManager::AddTCPClient(const std::string& arName, PhysLayerSettings s, const std::string& arAddr, boost::uint16_t aPort)
{
	mpImpl->AddTCPClient(arName, s, arAddr, aPort);
}
#endif

#if ENABLE_TCP_SERVER
void StackManager::AddTCPServer(const std::string& arName, PhysLayerSettings s, const std::string& arEndpoint, boost::uint16_t aPort)
{
	mpImpl->AddTCPServer(arName, s, arEndpoint, aPort);
}
#endif

#if ENABLE_SERIAL
void StackManager::AddSerial(const std::string& arName, PhysLayerSettings s, SerialSettings aSerial)
{
	mpImpl->AddSerial(arName, s, aSerial);
}
#endif

#if ENABLE_DNP3_MASTER
ICommandAcceptor* StackManager::AddMaster(const std::string& arPortName, const std::string& arStackName, FilterLevel aLevel,
                IDataObserver* apPublisher, const MasterStackConfig& arCfg)
{
	return mpImpl->AddMaster(arPortName, arStackName, aLevel, apPublisher, arCfg);
}
#endif

#if ENABLE_DNP3_SLAVE
IDataObserver* StackManager::AddSlave(const std::string& arPortName, const std::string& arStackName, FilterLevel aLevel,
                                      ICommandAcceptor* apCmdAcceptor, const SlaveStackConfig& arCfg)
{
	return mpImpl->AddSlave(arPortName, arStackName, aLevel, apCmdAcceptor, arCfg);
}
#endif

void StackManager::Shutdown()
{
	mpImpl->Shutdown();
}

void StackManager::RemovePort(const std::string& arPortName)
{
	mpImpl->RemovePort(arPortName);
}
void StackManager::RemoveStack(const std::string& arStackName)
{
	mpImpl->RemoveStack(arStackName);
}
std::vector<std::string> StackManager::GetStackNames()
{
	return mpImpl->GetStackNames();
}
std::vector<std::string> StackManager::GetPortNames()
{
	return mpImpl->GetPortNames();
}

}
}

