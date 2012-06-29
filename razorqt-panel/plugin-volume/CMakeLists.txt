cmake_minimum_required(VERSION 2.6)

set(PLUGIN "panelvolume")


set(HEADERS
    razorvolume.h
    volumebutton.h
    volumepopup.h
    pulseaudioengine.h
    pulseaudiodevice.h
    razorvolumeconfiguration.h
)

set(SOURCES
    razorvolume.cpp
    volumebutton.cpp
    volumepopup.cpp
    pulseaudioengine.cpp
    pulseaudiodevice.cpp
    razorvolumeconfiguration.cpp
)

set(MOCS
    razorvolume.h
    volumebutton.h
    volumepopup.h
    pulseaudioengine.h
    pulseaudiodevice.h
    razorvolumeconfiguration.h
)

set(UIS
    razorvolumeconfiguration.ui
)

LINK_LIBRARIES(${PLUGIN} ${VOLUME_LIB})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
