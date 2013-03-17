cmake_minimum_required(VERSION 2.6)

set(PLUGIN "teatime")

set(HEADERS
    teatimeplugin.h
)

set(SOURCES
    teatimeplugin.cpp
)

set(MOCS
    teatimeplugin.h
)

set(UIS
)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
