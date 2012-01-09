cmake_minimum_required(VERSION 2.6)

set(PLUGIN "panelhelloworld")


set(HEADERS
    razorhelloworld.h
)

set(SOURCES
    razorhelloworld.cpp
)

set(MOCS
    razorhelloworld.h
)

set(UIS
)


#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
