set(PLUGIN "mount")


set(HEADERS
    razormount.h
    mountbutton.h
    menudiskitem.h
    razormountconfiguration.h
)

set(SOURCES
    razormount.cpp
    mountbutton.cpp
    menudiskitem.cpp
    razormountconfiguration.cpp
)

set(MOCS
    razormount.h
    mountbutton.h
    menudiskitem.h
    razormountconfiguration.h
)

set(UIS
    menudiskitem.ui
    razormountconfiguration.ui
)

set(LIBRARIES razormount)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
