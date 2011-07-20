cmake_minimum_required(VERSION 2.6)

set(PLUGIN "mainmenu")


set(HEADERS
    razormainmenu.h
    menustyle.h
    razormainmenuconfiguration.h
)

set(SOURCES
    razormainmenu.cpp
    menustyle.cpp
    razormainmenuconfiguration.cpp
)

set(MOCS
    razormainmenu.h
    menustyle.h
    razormainmenuconfiguration.h
)

set(UIS
    razormainmenuconfiguration.ui
)

set(LIBRARIES
  razorqt
)

set(QT_USE_QTXML 1)
set(QT_USE_QTDBUS 1)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
