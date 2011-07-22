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

set(LIBRARIES ${LIBRARIES} ${CMAKE_BINARY_DIR}/librazorqt/librazorqxt.so )

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
