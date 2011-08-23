#
# Copyright (c) 2010, 2011 Green Energy Corp.
# All rights reserved.
#

ACLOCAL_AMFLAGS = -I config/autotools/m4

if ENABLE_DNP3_MASTER
DNP3_MASTER_CXXFLAGS=-DENABLE_DNP3_MASTER=1
else
DNP3_MASTER_CXXFLAGS=-DENABLE_DNP3_MASTER=0
endif

if ENABLE_DNP3_SLAVE
DNP3_SLAVE_CXXFLAGS=-DENABLE_DNP3_SLAVE=1
else
DNP3_SLAVE_CXXFLAGS=-DENABLE_DNP3_SLAVE=0
endif

AM_CXXFLAGS = $(BOOST_CPPFLAGS) -DBOOST_TEST_DYN_LINK -DBOOST_ASIO_ENABLE_CANCELIO -DBOOST_REGEX_NO_LIB $(DNP3_MASTER_CXXFLAGS) $(DNP3_SLAVE_CXXFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_DATE_TIME_LIB) $(BOOST_THREAD_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_UNIT_TEST_FRAMEWORK_LIB)

lib_LTLIBRARIES = libopendnp3_apl.la libopendnp3_dnp3.la libopendnp3_java.la

libopendnp3_apl_la_LDFLAGS  = -version-info 0:0:0 -Xlinker -Map=libopendnp3_apl.map
libopendnp3_apl_la_SOURCES  = APL/ASIOSerialHelpers.cpp
libopendnp3_apl_la_SOURCES += APL/AsyncLayerInterfaces.cpp
libopendnp3_apl_la_SOURCES += APL/AsyncResult.cpp
libopendnp3_apl_la_SOURCES += APL/AsyncTaskBase.cpp
libopendnp3_apl_la_SOURCES += APL/AsyncTaskContinuous.cpp
libopendnp3_apl_la_SOURCES += APL/AsyncTaskGroup.cpp
libopendnp3_apl_la_SOURCES += APL/AsyncTaskNonPeriodic.cpp
libopendnp3_apl_la_SOURCES += APL/AsyncTaskPeriodic.cpp
libopendnp3_apl_la_SOURCES += APL/AsyncTaskScheduler.cpp
libopendnp3_apl_la_SOURCES += APL/BaseDataTypes.cpp
libopendnp3_apl_la_SOURCES += APL/CommandManager.cpp
libopendnp3_apl_la_SOURCES += APL/CommandQueue.cpp
libopendnp3_apl_la_SOURCES += APL/CommandResponseQueue.cpp
libopendnp3_apl_la_SOURCES += APL/CommandTypes.cpp
libopendnp3_apl_la_SOURCES += APL/CopyableBuffer.cpp
libopendnp3_apl_la_SOURCES += APL/CRC.cpp
libopendnp3_apl_la_SOURCES += APL/EventLock.cpp
libopendnp3_apl_la_SOURCES += APL/Exception.cpp
libopendnp3_apl_la_SOURCES += APL/FlexibleDataObserver.cpp
libopendnp3_apl_la_SOURCES += APL/IHandlerAsync.cpp
libopendnp3_apl_la_SOURCES += APL/ITimerSource.cpp
libopendnp3_apl_la_SOURCES += APL/IOService.cpp
libopendnp3_apl_la_SOURCES += APL/IOServiceThread.cpp
libopendnp3_apl_la_SOURCES += APL/LockBase.cpp
libopendnp3_apl_la_SOURCES += APL/LockBoost.cpp
libopendnp3_apl_la_SOURCES += APL/Log.cpp
libopendnp3_apl_la_SOURCES += APL/LogEntryCircularBuffer.cpp
libopendnp3_apl_la_SOURCES += APL/LogEntry.cpp
libopendnp3_apl_la_SOURCES += APL/Loggable.cpp
libopendnp3_apl_la_SOURCES += APL/Logger.cpp
libopendnp3_apl_la_SOURCES += APL/LogToFile.cpp
libopendnp3_apl_la_SOURCES += APL/LogToStdio.cpp
libopendnp3_apl_la_SOURCES += APL/LogTypes.cpp
libopendnp3_apl_la_SOURCES += APL/LowerLayerToPhysAdapter.cpp
libopendnp3_apl_la_SOURCES += APL/MetricBuffer.cpp
libopendnp3_apl_la_SOURCES += APL/MultiplexingDataObserver.cpp
libopendnp3_apl_la_SOURCES += APL/PackingUnpacking.cpp
libopendnp3_apl_la_SOURCES += APL/Parsing.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerAsyncBase.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerAsyncBaseTCP.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerAsyncSerial.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerAsyncTCPClient.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerAsyncTCPServer.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerFactory.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerInstance.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerManager.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerMap.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerMonitor.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerMonitorStates.cpp
libopendnp3_apl_la_SOURCES += APL/PhysicalLayerStates.cpp
libopendnp3_apl_la_SOURCES += APL/PhysLoopback.cpp
libopendnp3_apl_la_SOURCES += APL/PostingNotifier.cpp
libopendnp3_apl_la_SOURCES += APL/PostingNotifierSource.cpp
libopendnp3_apl_la_SOURCES += APL/ProtocolUtil.cpp
libopendnp3_apl_la_SOURCES += APL/QualityConverter.cpp
libopendnp3_apl_la_SOURCES += APL/RandomizedBuffer.cpp
libopendnp3_apl_la_SOURCES += APL/ShiftableBuffer.cpp
libopendnp3_apl_la_SOURCES += APL/SuspendTimerSource.cpp
libopendnp3_apl_la_SOURCES += APL/Threadable.cpp
libopendnp3_apl_la_SOURCES += APL/ThreadBase.cpp
libopendnp3_apl_la_SOURCES += APL/ThreadBoost.cpp
libopendnp3_apl_la_SOURCES += APL/TimeBase.cpp
libopendnp3_apl_la_SOURCES += APL/TimeBoost.cpp
libopendnp3_apl_la_SOURCES += APL/Timeout.cpp
libopendnp3_apl_la_SOURCES += APL/TimerASIO.cpp
libopendnp3_apl_la_SOURCES += APL/TimerSourceASIO.cpp
libopendnp3_apl_la_SOURCES += APL/TimeSource.cpp
libopendnp3_apl_la_SOURCES += APL/TimingTools.cpp
libopendnp3_apl_la_SOURCES += APL/ToHex.cpp
libopendnp3_apl_la_SOURCES += APL/TrackingTaskGroup.cpp
libopendnp3_apl_la_SOURCES += APL/Util.cpp

libopendnp3_dnp3_la_LDFLAGS = -version-info 0:0:0 -Xlinker -Map=libopendnp3_dnp3.map
libopendnp3_dnp3_la_LIBADD  = libopendnp3_apl.la
libopendnp3_dnp3_la_SOURCES = DNP3/AlwaysOpeningVtoRouter.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/APDUConstants.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/APDU.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/AppChannelStates.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/AppHeader.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/AppInterfaces.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/AppLayerChannel.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/AppLayer.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/AsyncStackManager.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/BufferTypes.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/ClassCounter.cpp
if ENABLE_DNP3_MASTER
libopendnp3_dnp3_la_SOURCES += DNP3/ControlTasks.cpp
endif
libopendnp3_dnp3_la_SOURCES += DNP3/Database.cpp
if ENABLE_DNP3_MASTER
libopendnp3_dnp3_la_SOURCES += DNP3/DataPoll.cpp
endif
libopendnp3_dnp3_la_SOURCES += DNP3/DeviceTemplate.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/DNPCommandMaster.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/DNPCrc.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/EnhancedVto.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/EnhancedVtoRouter.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/HeaderReadIterator.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/IndexedWriteIterator.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/IStackObserver.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/LinkChannel.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/LinkFrame.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/LinkHeader.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/LinkLayerConstants.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/LinkLayer.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/LinkLayerReceiver.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/LinkLayerRouter.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/LinkReceiverStates.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/LinkRoute.cpp
if ENABLE_DNP3_MASTER
libopendnp3_dnp3_la_SOURCES += DNP3/Master.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/MasterSchedule.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/MasterStack.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/MasterStates.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/MasterTaskBase.cpp
endif
libopendnp3_dnp3_la_SOURCES += DNP3/ObjectHeader.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/ObjectInterfaces.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/ObjectReadIterator.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/Objects.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/ObjectWriteIterator.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/PointClass.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/PriLinkLayerStates.cpp
if ENABLE_DNP3_SLAVE
libopendnp3_dnp3_la_SOURCES += DNP3/ResponseContext.cpp
endif
if ENABLE_DNP3_MASTER
libopendnp3_dnp3_la_SOURCES += DNP3/ResponseLoader.cpp
endif
libopendnp3_dnp3_la_SOURCES += DNP3/SecLinkLayerStates.cpp
if ENABLE_DNP3_SLAVE
libopendnp3_dnp3_la_SOURCES += DNP3/SlaveConfig.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/Slave.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/SlaveEventBuffer.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/SlaveResponseTypes.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/SlaveStack.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/SlaveStates.cpp
endif
libopendnp3_dnp3_la_SOURCES += DNP3/SolicitedChannel.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/Stack.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/StackManager.cpp
if ENABLE_DNP3_MASTER
libopendnp3_dnp3_la_SOURCES += DNP3/StartupTasks.cpp
endif
libopendnp3_dnp3_la_SOURCES += DNP3/TLS_Base.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/TransportLayer.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/TransportRx.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/TransportStates.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/TransportTx.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/UnsolicitedChannel.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/VtoData.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/VtoReader.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/VtoRouter.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/VtoRouterManager.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/VtoRouterSettings.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/VtoTransmitTask.cpp
libopendnp3_dnp3_la_SOURCES += DNP3/VtoWriter.cpp

libopendnp3_java_la_LDFLAGS = -version-info 0:0:0
libopendnp3_java_la_SOURCES = DNP3Java/JavaDNP3.i

nobase_pkginclude_HEADERS= \
	version.hpp \
	APL/ASIOSerialHelpers.h \
	APL/AsyncLayerInterfaces.h \
	APL/AsyncTaskBase.h \
	APL/AsyncTaskContinuous.h \
	APL/AsyncTaskGroup.h \
	APL/AsyncTaskInterfaces.h \
	APL/AsyncTaskNonPeriodic.h \
	APL/AsyncTaskPeriodic.h \
	APL/AsyncTaskScheduler.h \
	APL/BaseDataTypes.h \
	APL/BoundNotifier.h \
	APL/CachedLogVariable.h \
	APL/ChangeBuffer.h \
	APL/CommandInterfaces.h \
	APL/CommandManager.h \
	APL/CommandQueue.h \
	APL/CommandResponseQueue.h \
	APL/CommandTypes.h \
	APL/Configure.h \
	APL/CopyableBuffer.h \
	APL/CRC.h \
	APL/DataInterfaces.h \
	APL/DataTypes.h \
	APL/DeleteAny.h \
	APL/EventLockBase.h \
	APL/EventLock.h \
	APL/EventSet.h \
	APL/Exception.h \
	APL/FlexibleDataObserver.h \
	APL/GetKeys.h \
	APL/IEventLock.h \
	APL/IHandlerAsync.h \
	APL/INotifier.h \
	APL/IOService.h \
	APL/IOServiceThread.h \
	APL/IPhysicalLayerAsync.h \
	APL/IPhysicalLayerObserver.h \
	APL/IPhysicalLayerSource.h \
	APL/ISubject.h \
	APL/ITimer.h \
	APL/ITimerSource.h \
	APL/ITimeSource.h \
	APL/ITransactable.h \
	APL/LockBase.h \
	APL/LockBoost.h \
	APL/Lock.h \
	APL/LogBase.h \
	APL/LogEntryCircularBuffer.h \
	APL/LogEntry.h \
	APL/Loggable.h \
	APL/Logger.h \
	APL/Log.h \
	APL/LogToFile.h \
	APL/LogToStdio.h \
	APL/LogTypes.h \
	APL/LogVar.h \
	APL/LowerLayerToPhysAdapter.h \
	APL/MetricBuffer.h \
	APL/MultiplexingDataObserver.h \
	APL/Notifier.h \
	APL/PackingTemplates.h \
	APL/PackingUnpacking.h \
	APL/Parsing.h \
	APL/PhysicalLayerAsyncASIO.h \
	APL/PhysicalLayerAsyncBase.h \
	APL/PhysicalLayerAsyncBaseTCP.h \
	APL/PhysicalLayerAsyncSerial.h \
	APL/PhysicalLayerAsyncTCPClient.h \
	APL/PhysicalLayerAsyncTCPServer.h \
	APL/PhysicalLayerFactory.h \
	APL/PhysicalLayerFunctors.h \
	APL/PhysicalLayerInstance.h \
	APL/PhysicalLayerManager.h \
	APL/PhysicalLayerMap.h \
	APL/PhysicalLayerMonitor.h \
	APL/PhysicalLayerMonitorStates.h \
	APL/PhysicalLayerStates.h \
	APL/PhysLayerSettings.h \
	APL/PhysLoopback.h \
	APL/PostingNotifier.h \
	APL/PostingNotifierSource.h \
	APL/ProtocolUtil.h \
	APL/QualityConverter.h \
	APL/QualityMasks.h \
	APL/QueueingFDO.h \
	APL/Random.h \
	APL/RandomizedBuffer.h \
	APL/SerialTypes.h \
	APL/ShiftableBuffer.h \
	APL/Singleton.h \
	APL/SubjectBase.h \
	APL/SuspendTimerSource.h \
	APL/SyncVar.h \
	APL/Threadable.h \
	APL/ThreadBase.h \
	APL/ThreadBoost.h \
	APL/Thread.h \
	APL/TimeBase.h \
	APL/TimeBoost.h \
	APL/Timeout.h \
	APL/TimerASIO.h \
	APL/TimerSourceASIO.h \
	APL/TimeSource.h \
	APL/TimeTypes.h \
	APL/TimingTools.h \
	APL/ToHex.h \
	APL/TrackingTaskGroup.h \
	APL/Types.h \
	APL/Uncopyable.h \
	APL/Util.h \
	DNP3/AlwaysOpeningVtoRouter.h \
	DNP3/APDUConstants.h \
	DNP3/APDU.h \
	DNP3/AppChannelStates.h \
	DNP3/AppConfig.h \
	DNP3/AppHeader.h \
	DNP3/AppInterfaces.h \
	DNP3/AppLayerChannel.h \
	DNP3/AppLayer.h \
	DNP3/AsyncStackManager.h \
	DNP3/BufferSetTypes.h \
	DNP3/BufferTypes.h \
	DNP3/ClassCounter.h \
	DNP3/ClassMask.h \
	DNP3/ControlTasks.h \
	DNP3/CTOHistory.h \
	DNP3/Database.h \
	DNP3/DatabaseInterfaces.h \
	DNP3/DataPoll.h \
	DNP3/DeviceTemplate.h \
	DNP3/DeviceTemplateTypes.h \
	DNP3/DNPCommandMaster.h \
	DNP3/DNPConstants.h \
	DNP3/DNPCrc.h \
	DNP3/DNPDatabaseTypes.h \
	DNP3/DNPExceptions.h \
	DNP3/DNPFromStream.h \
	DNP3/DNPToStream.h \
	DNP3/EnhancedVto.h \
	DNP3/EnhancedVtoRouter.h \
	DNP3/EventBufferBase.h \
	DNP3/EventBuffers.h \
	DNP3/EventTypes.h \
	DNP3/HeaderReadIterator.h \
	DNP3/IFrameSink.h \
	DNP3/ILinkContext.h \
	DNP3/ILinkRouter.h \
	DNP3/IndexedWriteIterator.h \
	DNP3/IStackObserver.h \
	DNP3/IVtoEventAcceptor.h \
	DNP3/LinkChannel.h \
	DNP3/LinkConfig.h \
	DNP3/LinkFrame.h \
	DNP3/LinkHeader.h \
	DNP3/LinkLayerConstants.h \
	DNP3/LinkLayer.h \
	DNP3/LinkLayerReceiver.h \
	DNP3/LinkLayerRouter.h \
	DNP3/LinkReceiverStates.h \
	DNP3/LinkRoute.h \
	DNP3/MasterConfig.h \
	DNP3/MasterConfigTypes.h \
	DNP3/Master.h \
	DNP3/MasterSchedule.h \
	DNP3/MasterStackConfig.h \
	DNP3/MasterStack.h \
	DNP3/MasterStates.h \
	DNP3/MasterTaskBase.h \
	DNP3/ObjectHeader.h \
	DNP3/ObjectInterfaces.h \
	DNP3/ObjectReadIterator.h \
	DNP3/Objects.h \
	DNP3/ObjectWriteIterator.h \
	DNP3/PointClass.h \
	DNP3/PriLinkLayerStates.h \
	DNP3/ResponseContext.h \
	DNP3/ResponseLoader.h \
	DNP3/SecLinkLayerStates.h \
	DNP3/SlaveConfig.h \
	DNP3/SlaveEventBuffer.h \
	DNP3/Slave.h \
	DNP3/SlaveResponseTypes.h \
	DNP3/SlaveStackConfig.h \
	DNP3/SlaveStack.h \
	DNP3/SlaveStates.h \
	DNP3/SolicitedChannel.h \
	DNP3/Stack.h \
	DNP3/StackManager.h \
	DNP3/StartupTasks.h \
	DNP3/TLS_Base.h \
	DNP3/TransportConstants.h \
	DNP3/TransportLayer.h \
	DNP3/TransportRx.h \
	DNP3/TransportStates.h \
	DNP3/TransportTx.h \
	DNP3/UnsolicitedChannel.h \
	DNP3/VtoConfig.h \
	DNP3/VtoData.h \
	DNP3/VtoDataInterface.h \
	DNP3/VtoEventBufferAdapter.h \
	DNP3/VtoReader.h \
	DNP3/VtoRouter.h \
	DNP3/VtoRouterManager.h \
	DNP3/VtoRouterSettings.h \
	DNP3/VtoTransmitTask.h \
	DNP3/VtoWriter.h

bin_PROGRAMS = testset

testset_LDADD = libopendnp3_apl.la libopendnp3_dnp3.la
testset_CFLAGS = -Itinyxml
testset_SOURCES = \
	APLXML/PhysicalLayerManagerXML.cpp \
	APLXML/PhysicalLayerXMLFactory.cpp \
	APLXML/tinybinding.cpp \
	APLXML/XML_APL.cpp \
	APLXML/XMLConversion.cpp \
	DNP3XML/XML_DNP3.cpp \
	DNP3XML/XML_TestSet.cpp \
	DNP3XML/XmlToConfig.cpp \
	Terminal/ControlResponseTE.cpp \
	Terminal/ControlTerminalExtension.cpp \
	Terminal/DOTerminalExtension.cpp \
	Terminal/FlexibleObserverTerminalExtension.cpp \
	Terminal/LineReader.cpp \
	Terminal/LogTerminalExtension.cpp \
	Terminal/PhysicalLayerIOStreamAsync.cpp \
	Terminal/PhysicalLayerSyncProxy.cpp \
	Terminal/Terminal.cpp \
	Terminal/TerminalInterfaces.cpp \
	TestSet/AddressScanner.cpp \
	TestSet/main.cpp \
	TestSet/StackHelpers.cpp \
	XMLBindings/APLXML_Base.cpp \
	XMLBindings/APLXML_DNP.cpp \
	XMLBindings/APLXML_MTS.cpp \
	XMLBindings/APLXML_STS.cpp \
	tinyxml/tinyxml.cpp \
	tinyxml/tinyxmlerror.cpp \
	tinyxml/tinyxmlparser.cpp

noinst_PROGRAMS = dnp3test

dnp3test_LDADD   = libopendnp3_apl.la libopendnp3_dnp3.la
dnp3test_SOURCES = APLTestTools/AsyncPhysTestObject.cpp
dnp3test_SOURCES += APLTestTools/AsyncTestObjectASIO.cpp
dnp3test_SOURCES += APLTestTools/AsyncTestObject.cpp
dnp3test_SOURCES += APLTestTools/BufferHelpers.cpp
dnp3test_SOURCES += APLTestTools/BufferTestObject.cpp
dnp3test_SOURCES += APLTestTools/LogTester.cpp
dnp3test_SOURCES += APLTestTools/LoopbackPhysicalLayerAsync.cpp
dnp3test_SOURCES += APLTestTools/MockLowerLayer.cpp
dnp3test_SOURCES += APLTestTools/MockPhysicalLayerAsync.cpp
dnp3test_SOURCES += APLTestTools/MockPhysicalLayerAsyncTS.cpp
dnp3test_SOURCES += APLTestTools/MockPhysicalLayerMonitor.cpp
dnp3test_SOURCES += APLTestTools/MockPhysicalLayerSource.cpp
dnp3test_SOURCES += APLTestTools/MockResponseAcceptor.cpp
dnp3test_SOURCES += APLTestTools/MockTimerSource.cpp
dnp3test_SOURCES += APLTestTools/MockUpperLayer.cpp
dnp3test_SOURCES += DNP3Test/AppLayerTest.cpp
dnp3test_SOURCES += DNP3Test/ComparingDataObserver.cpp
dnp3test_SOURCES += DNP3Test/DNPHelpers.cpp
if ENABLE_DNP3_MASTER
if ENABLE_DNP3_SLAVE
dnp3test_SOURCES += DNP3Test/IntegrationTest.cpp
endif
endif
dnp3test_SOURCES += DNP3Test/LinkLayerRouterTest.cpp
dnp3test_SOURCES += DNP3Test/LinkLayerTest.cpp
if ENABLE_DNP3_MASTER
dnp3test_SOURCES += DNP3Test/MasterTestObject.cpp
endif
dnp3test_SOURCES += DNP3Test/MockAppLayer.cpp
dnp3test_SOURCES += DNP3Test/MockAppUser.cpp
dnp3test_SOURCES += DNP3Test/MockFrameSink.cpp
if ENABLE_DNP3_MASTER
dnp3test_SOURCES += DNP3Test/ResponseLoaderTestObject.cpp
endif
if ENABLE_DNP3_SLAVE
dnp3test_SOURCES += DNP3Test/SlaveTestObject.cpp
endif
if ENABLE_DNP3_MASTER
dnp3test_SOURCES += DNP3Test/StartupTeardownTest.cpp
endif
dnp3test_SOURCES += DNP3Test/TestAPDU.cpp
dnp3test_SOURCES += DNP3Test/TestAPDUWriting.cpp
dnp3test_SOURCES += DNP3Test/TestAppLayer.cpp
dnp3test_SOURCES += DNP3Test/TestCRC.cpp
dnp3test_SOURCES += DNP3Test/TestDatabase.cpp
dnp3test_SOURCES += DNP3Test/TestEnhancedVtoRouter.cpp
dnp3test_SOURCES += DNP3Test/TestEventBufferBase.cpp
dnp3test_SOURCES += DNP3Test/TestEventBuffers.cpp
if ENABLE_DNP3_MASTER
if ENABLE_DNP3_SLAVE
dnp3test_SOURCES += DNP3Test/TestIntegration.cpp
endif
endif
dnp3test_SOURCES += DNP3Test/TestLinkFrameDNP.cpp
dnp3test_SOURCES += DNP3Test/TestLinkLayer.cpp
dnp3test_SOURCES += DNP3Test/TestLinkLayerRouter.cpp
dnp3test_SOURCES += DNP3Test/TestLinkReceiver.cpp
dnp3test_SOURCES += DNP3Test/TestLinkRoute.cpp
if ENABLE_DNP3_MASTER
dnp3test_SOURCES += DNP3Test/TestMaster.cpp
endif
dnp3test_SOURCES += DNP3Test/TestObjects.cpp
if ENABLE_DNP3_MASTER
dnp3test_SOURCES += DNP3Test/TestResponseLoader.cpp
endif
if ENABLE_DNP3_SLAVE
dnp3test_SOURCES += DNP3Test/TestSlave.cpp
dnp3test_SOURCES += DNP3Test/TestSlaveEventBuffer.cpp
endif
dnp3test_SOURCES += DNP3Test/TestStartBoostUTF.cpp
if ENABLE_DNP3_MASTER
dnp3test_SOURCES += DNP3Test/TestStartupTeardown.cpp
endif
dnp3test_SOURCES += DNP3Test/TestTransportLayer.cpp
dnp3test_SOURCES += DNP3Test/TestTransportLoopback.cpp
dnp3test_SOURCES += DNP3Test/TestTransportScalability.cpp
dnp3test_SOURCES += DNP3Test/TestVtoInterface.cpp
if ENABLE_DNP3_MASTER
if ENABLE_DNP3_SLAVE
dnp3test_SOURCES += DNP3Test/TestVtoLoopbackIntegration.cpp
dnp3test_SOURCES += DNP3Test/TestVtoOnewayIntegration.cpp
endif
endif
dnp3test_SOURCES += DNP3Test/TestVtoRouter.cpp
dnp3test_SOURCES += DNP3Test/TestVtoRouterManager.cpp
dnp3test_SOURCES += DNP3Test/TestVtoWriter.cpp
dnp3test_SOURCES += DNP3Test/TransportIntegrationStack.cpp
dnp3test_SOURCES += DNP3Test/TransportLoopbackTestObject.cpp
dnp3test_SOURCES += DNP3Test/TransportScalabilityTestObject.cpp
dnp3test_SOURCES += DNP3Test/TransportStackPair.cpp
dnp3test_SOURCES += DNP3Test/TransportTestObject.cpp
if ENABLE_DNP3_MASTER
if ENABLE_DNP3_SLAVE
dnp3test_SOURCES += DNP3Test/VtoIntegrationTestBase.cpp
endif
endif

ASTYLE=astyle --options=config/astyle.cfg

DOCBOOK2HTML=docbook2html
DOCBOOK2PDF=docbook2pdf

astyle:
	${ASTYLE}   ${srcdir}/APL/*.cpp            ${srcdir}/APL/*.h
	${ASTYLE}   ${srcdir}/APLTestTools/*.cpp   ${srcdir}/APLTestTools/*.h
	${ASTYLE}   ${srcdir}/DNP3/*.cpp           ${srcdir}/DNP3/*.h
	${ASTYLE}   ${srcdir}/DNP3Test/*.cpp       ${srcdir}/DNP3Test/*.h

docs: docs-html docs-pdf

docs-html:
	${DOCBOOK2HTML} -o docs/html/ docs/main.xml

docs-pdf:
	${DOCBOOK2PDF} -o docs/pdf/ docs/main.xml

include config/autotools/mk/doxygen.mak
include config/autotools/mk/lcov.mak

.PHONY : astyle docs docs-html docs-pdf lcov lcov-report lcov-reset doxygen-run doxygen-doc $(DX_PS_GOAL) $(DX_PDF_GOAL)

