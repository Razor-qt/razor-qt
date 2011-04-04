set(PLUGIN "tray2")

set(HEADERS
  razortray.h
)

set(SOURCES
  razortray.cpp
)

set(MOCS
  razortray.h
)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})

