cmake_minimum_required(VERSION 2.6)

set(PLUGIN "panelkbindicator")


set(HEADERS
    razorkbindicator.h
    razorkbindicatoreventfilter.h
    razorkbindicatorconfiguration.h
)

set(SOURCES
    razorkbindicator.cpp
    razorkbindicatoreventfilter.cpp
    razorkbindicatorconfiguration.cpp
)

set(MOCS
    razorkbindicator.h
    razorkbindicatoreventfilter.h
    razorkbindicatorconfiguration.h
)

set(UIS
    razorkbindicatorconfiguration.ui
)


#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
