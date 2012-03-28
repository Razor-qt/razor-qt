cmake_minimum_required(VERSION 2.6)

set(PLUGIN "networkmonitor")


set(HEADERS
	razornetworkmonitor.h
	razornetworkmonitorconfiguration.h
)

set(SOURCES
	razornetworkmonitor.cpp
	razornetworkmonitorconfiguration.cpp
)

set(MOCS
	razornetworkmonitor.h
	razornetworkmonitorconfiguration.h
)

set(UIS
	razornetworkmonitorconfiguration.ui
)

set(RESOURCES
  resources.qrc
)

LINK_LIBRARIES(${PLUGIN} ${STATGRAB_LIB})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
