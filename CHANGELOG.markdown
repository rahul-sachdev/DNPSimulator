Change Log
==============

### 0.9.4 ###
Rebased java namespace to org.totalgrid.reef.protocol.dnp3
Java SO static loading can be overriden using system property "org.totalgrid.reef.protocol.dnp3.nostaticload"
Fixed a major bug in slave unsolcited mode on event buffer overflow.


### 0.9.3 ###
- Fixed parsing bug in testset that caused erroneous pulse repititions
- Fixed a bug in DataPoll.cpp that caused stack to crash if it eas configured with an empty exception scan
- Added an address scanning feature to the TestSet
- Removed ASIOIncludes.h and replaced with preprocessor defines for all platforms.
- Fix for issue #9
- Jar now require shared library to be install on java.library.path



