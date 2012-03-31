cmake_minimum_required(VERSION 2.6)

set(PLUGIN "sensors")


set(HEADERS
    chip.h
    feature.h
    razorsensors.h
    razorsensorsconfiguration.h
    sensors.h
)

set(SOURCES
    chip.cpp
    feature.cpp
    razorsensors.cpp
    razorsensorsconfiguration.cpp
    sensors.cpp
)

set(MOCS
    razorsensors.h
    razorsensorsconfiguration.h
)

set(UIS
  razorsensorsconfiguration.ui
)

LINK_LIBRARIES(${PLUGIN} ${SENSORS_LIB})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
