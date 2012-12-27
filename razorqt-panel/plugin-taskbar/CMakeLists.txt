cmake_minimum_required(VERSION 2.6)

set(PLUGIN "taskbar")


set(HEADERS
    razortaskbar.h
    razortaskbutton.h
    razortaskbarconfiguration.h
    razortaskbarplugin.h
)

set(SOURCES
    razortaskbar.cpp
    razortaskbutton.cpp
    razortaskbarconfiguration.cpp
    razortaskbarplugin.cpp
)

set(MOCS
    razortaskbar.h
    razortaskbutton.h
    razortaskbarconfiguration.h
    razortaskbarplugin.h
)

set(UIS
    razortaskbarconfiguration.ui
)

set(LIBRARIES
    razorqt
)


#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
