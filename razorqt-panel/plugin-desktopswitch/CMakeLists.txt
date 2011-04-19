cmake_minimum_required(VERSION 2.6)

set(PLUGIN "desktopswitch")


set(HEADERS
  desktopswitch.h
)

set(SOURCES
  desktopswitch.cpp
)

set(MOCS
  desktopswitch.h
)


#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
