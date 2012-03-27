cmake_minimum_required(VERSION 2.6)

set(PLUGIN "cpuload")


set(HEADERS
	razorcpuload.h
	razorcpuloadconfiguration.h
)

set(SOURCES
	razorcpuload.cpp
	razorcpuloadconfiguration.cpp
)

set(MOCS
	razorcpuload.h
	razorcpuloadconfiguration.h
)

set(UIS
	razorcpuloadconfiguration.ui
)

LINK_LIBRARIES(${PLUGIN} ${STATGRAB_LIB})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
