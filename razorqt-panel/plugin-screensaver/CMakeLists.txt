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

set(LIBRARIES ${LIBRARIES} razor-global-key-shortcuts-client )

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
