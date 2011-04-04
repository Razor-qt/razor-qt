cmake_minimum_required(VERSION 2.6)

set(PLUGIN "clock2")


set(HEADERS
  razorclock.h
)

set(SOURCES
  razorclock.cpp
)

set(MOCS
  razorclock.h
)


#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
