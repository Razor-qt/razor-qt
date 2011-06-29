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

set(LIBRARIES ${LIBRARIES} ${CMAKE_BINARY_DIR}/librazorqt/librazorqxt.so )

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
