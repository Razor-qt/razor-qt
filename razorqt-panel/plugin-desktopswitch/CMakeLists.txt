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

set(LIBRARIES ${LIBRARIES} ${CMAKE_BINARY_DIR}/librazorqt/librazorqxt.so )

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
