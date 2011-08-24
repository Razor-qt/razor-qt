set(PLUGIN "mount")


set(HEADERS
    razormount.h
    mountbutton.h
    menudiskitem.h
)

set(SOURCES
    razormount.cpp
    mountbutton.cpp
    menudiskitem.cpp
)

set(MOCS
    razormount.h
    mountbutton.h
    menudiskitem.h
)

set(UIS
    menudiskitem.ui
)

set(LIBRARIES razormount)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
