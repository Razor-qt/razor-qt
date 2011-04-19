cmake_minimum_required(VERSION 2.6)

set(PLUGIN "taskbar")


set(HEADERS
  razortaskbar.h
  razortaskbutton.h
)

set(SOURCES
  razortaskbar.cpp
  razortaskbutton.cpp
)

set(MOCS
  razortaskbar.h
  razortaskbutton.h
)

set(LIBRARIES
  razorqt
)


#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
