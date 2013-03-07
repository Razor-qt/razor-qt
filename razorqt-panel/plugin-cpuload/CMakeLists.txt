cmake_minimum_required(VERSION 2.6)

set(PLUGIN "cpuload")


set(HEADERS
        razorcpuloadplugin.h
	razorcpuload.h
	razorcpuloadconfiguration.h
)

set(SOURCES
        razorcpuloadplugin.cpp
	razorcpuload.cpp
	razorcpuloadconfiguration.cpp
)

set(MOCS
        razorcpuloadplugin.h
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
