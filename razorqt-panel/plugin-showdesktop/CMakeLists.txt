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

set(LIBRARIES ${LIBRARIES} razorqxt )

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
