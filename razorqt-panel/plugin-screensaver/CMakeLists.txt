set(PLUGIN "screensaver")

set(HEADERS
  panelscreensaver.h
)

set(SOURCES
  panelscreensaver.cpp
)

set(MOCS
  panelscreensaver.h
)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../../librazorqt
    ${CMAKE_CURRENT_SOURCE_DIR}/../panel
)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
