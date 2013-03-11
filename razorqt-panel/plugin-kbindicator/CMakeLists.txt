cmake_minimum_required(VERSION 2.6)

set(PLUGIN "panelkbindicator")


set(HEADERS
    razorkbindicator.h
    razorkbindicatorconfiguration.h
)

set(SOURCES
    razorkbindicator.cpp
    razorkbindicatorconfiguration.cpp
)

set(MOCS
    razorkbindicator.h
    razorkbindicatorconfiguration.h
)

set(UIS
    razorkbindicatorconfiguration.ui
)


#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
