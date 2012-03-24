cmake_minimum_required(VERSION 2.6)

set(PLUGIN "nemo")


set(HEADERS
	razornemo.h
	razornemoconfiguration.h
)

set(SOURCES
	razornemo.cpp
	razornemoconfiguration.cpp
)

set(MOCS
	razornemo.h
	razornemoconfiguration.h
)

set(UIS
	razornemoconfiguration.ui
)

set(RESOURCES
  resources.qrc
)

LINK_LIBRARIES(${PLUGIN} ${STATGRAB_LIB})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
