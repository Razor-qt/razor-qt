set(PLUGIN "showdesktop")

set(HEADERS
  showdesktop.h
)

set(SOURCES
  showdesktop.cpp
)

set(MOCS
  showdesktop.h
)

set(LIBRARIES ${LIBRARIES} razor-global-key-shortcuts-client )

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
