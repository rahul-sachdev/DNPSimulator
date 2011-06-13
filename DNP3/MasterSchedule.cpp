/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership.  Green Enery
 * Corp licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "MasterSchedule.h"
#include "Master.h"

#include <APL/AsyncTaskBase.h>
#include <APL/AsyncTaskContinuous.h>
#include <APL/AsyncTaskGroup.h>

#include <boost/foreach.hpp>

namespace apl
{
namespace dnp
{

MasterSchedule::MasterSchedule(AsyncTaskGroup* apGroup) :
	mpGroup(apGroup)
{}

void MasterSchedule::EnableOnlineTasks()
{
	mpGroup->Enable(ONLINE_ONLY_TASKS);
}

void MasterSchedule::DisableOnlineTasks()
{
	mpGroup->Disable(ONLINE_ONLY_TASKS);
}

void MasterSchedule::ResetStartupTasks()
{
	mpGroup->ResetTasks(START_UP_TASKS);
}

MasterSchedule MasterSchedule::GetSchedule(MasterConfig aCfg,
        Master* apMaster,
        AsyncTaskGroup* apGroup)
{
	AsyncTaskBase* pIntegrity = apGroup->Add(
	                                aCfg.IntegrityRate,
	                                aCfg.TaskRetryRate,
	                                AMP_POLL,
	                                bind(&Master::IntegrityPoll, apMaster, _1),
	                                "Integrity Poll");

	pIntegrity->SetFlags(ONLINE_ONLY_TASKS | START_UP_TASKS);

	if (aCfg.DoUnsolOnStartup) {
		/*
		 * DNP3Spec-V2-Part2-ApplicationLayer-_20090315.pdf, page 8
		 * says that UNSOL should be disabled before an integrity scan
		 * is done.
		 */
		TaskHandler handler = bind(&Master::ChangeUnsol,
		                           apMaster,
		                           _1,
		                           false,
		                           PC_ALL_EVENTS);
		AsyncTaskBase* pUnsolDisable = apGroup->Add(
		                                   -1,
		                                   aCfg.
		                                   TaskRetryRate,
		                                   AMP_UNSOL_CHANGE,
		                                   handler,
		                                   "Unsol Disable");

		pUnsolDisable->SetFlags(ONLINE_ONLY_TASKS | START_UP_TASKS);
		pIntegrity->AddDependency(pUnsolDisable);

		if (aCfg.EnableUnsol) {
			TaskHandler handler = bind(
			                          &Master::ChangeUnsol,
			                          apMaster,
			                          _1,
			                          true,
			                          aCfg.UnsolClassMask);

			AsyncTaskBase* pUnsolEnable = apGroup->Add(
			                                  -1,
			                                  aCfg.TaskRetryRate,
			                                  AMP_UNSOL_CHANGE,
			                                  handler,
			                                  "Unsol Enable");

			pUnsolEnable->SetFlags(ONLINE_ONLY_TASKS | START_UP_TASKS);
			pUnsolEnable->AddDependency(pIntegrity);
		}
	}

	/*
	 * Load any exception scans and make them dependent on the
	 * integrity poll.
	 */
	BOOST_FOREACH(ExceptionScan e, aCfg.mScans) {
		AsyncTaskBase* pEventScan = apGroup->Add(
		                                e.ScanRate,
		                                aCfg.TaskRetryRate,
		                                AMP_POLL,
		                                bind(&Master::EventPoll, apMaster, _1, e.ClassMask),
		                                "Event Scan");

		pEventScan->SetFlags(ONLINE_ONLY_TASKS);
		pEventScan->AddDependency(pIntegrity);
	}

	MasterSchedule schedule(apGroup);

	/* Tasks are executed when the master is is idle */
	schedule.mpCommandTask = apGroup->AddContinuous(
	                             AMP_COMMAND,
	                             boost::bind(&Master::ProcessCommand, apMaster, _1),
	                             "Command");

	schedule.mpTimeTask = apGroup->AddContinuous(
	                          AMP_TIME_SYNC,
	                          boost::bind(&Master::SyncTime, apMaster, _1),
	                          "TimeSync");

	schedule.mpClearRestartTask = apGroup->AddContinuous(
	                                  AMP_CLEAR_RESTART,
	                                  boost::bind(&Master::WriteIIN, apMaster, _1),
	                                  "Clear IIN");

	schedule.mpVtoTransmitTask = apGroup->AddContinuous(
	                                 AMP_VTO_TRANSMIT,
	                                 boost::bind(&Master::TransmitVtoData, apMaster, _1),
	                                 "Buffer VTO Data");

	schedule.mpTimeTask->SetFlags(ONLINE_ONLY_TASKS);
	schedule.mpClearRestartTask->SetFlags(ONLINE_ONLY_TASKS);

	return schedule;
}

}
}

/* vim: set ts=4 sw=4: */

