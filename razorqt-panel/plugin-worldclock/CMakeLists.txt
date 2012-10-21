cmake_minimum_required(VERSION 2.6)

set(PLUGIN "panelworldclock")


set(HEADERS
    razorworldclock.h
    razorworldclockconfiguration.h
)

set(SOURCES
    razorworldclock.cpp
    razorworldclockconfiguration.cpp
)

set(MOCS
    razorworldclock.h
    razorworldclockconfiguration.h
)

set(UIS
    razorworldclockconfiguration.ui
)


include_directories(${ICU_INCLUDE_DIRS})
LINK_LIBRARIES(${PLUGIN} ${ICU_LIBRARIES})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
