cmake_minimum_required(VERSION 2.6)

set(PLUGIN "cpuload")


set(HEADERS
	razorcpuload.h
)

set(SOURCES
	razorcpuload.cpp
)

set(MOCS
	razorcpuload.h
)

set(UIS
)

LINK_LIBRARIES(${PLUGIN} ${STATGRAB_LIB})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
