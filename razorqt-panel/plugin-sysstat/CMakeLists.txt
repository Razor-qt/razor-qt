cmake_minimum_required(VERSION 2.6)

set(PLUGIN "panelsysstat")


set(HEADERS
    razorsysstat.h
    razorsysstatconfiguration.h
    razorsysstatutils.h
)

set(SOURCES
    razorsysstat.cpp
    razorsysstatconfiguration.cpp
    razorsysstatutils.cpp
)

set(MOCS
    razorsysstat.h
    razorsysstatconfiguration.h
)

set(UIS
    razorsysstatconfiguration.ui
)


LINK_LIBRARIES(${PLUGIN} ${SYSSTAT_LIB})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(panelsysstat_LIBRARIES
    sysstat
)
target_link_libraries(${PLUGIN}  ${panelsysstat_LIBRARIES})
