cmake_minimum_required(VERSION 2.6)

set(PLUGIN "desktopswitch")


set(HEADERS
  desktopswitch.h
  desktopswitchbutton.h
)

set(SOURCES
  desktopswitch.cpp
  desktopswitchbutton.cpp
)

set(MOCS
  desktopswitch.h
  desktopswitchbutton.h
)

set(LIBRARIES ${LIBRARIES} razor-global-key-shortcuts-client )

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
