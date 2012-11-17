cmake_minimum_required(VERSION 2.6)

set(PLUGIN "clock")

include(ConfigureChecks.cmake)
configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(HEADERS
    razorclock.h
    razorclockconfiguration.h
    calendar_utils.h
)

set(SOURCES
    razorclock.cpp
    razorclockconfiguration.cpp
    calendar_utils.cpp
)

set(MOCS
    razorclock.h
    razorclockconfiguration.h
)

set(UIS
    razorclockconfiguration.ui
)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
