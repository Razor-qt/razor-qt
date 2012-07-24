cmake_minimum_required(VERSION 2.6)

set(PLUGIN "panelvolume")


set(HEADERS
    razorvolume.h
    volumebutton.h
    volumepopup.h
    audiodevice.h
    razorvolumeconfiguration.h
    audioengine.h
)

set(SOURCES
    razorvolume.cpp
    volumebutton.cpp
    volumepopup.cpp
    audiodevice.cpp
    razorvolumeconfiguration.cpp
    audioengine.cpp
)

set(MOCS
    razorvolume.h
    volumebutton.h
    volumepopup.h
    audiodevice.h
    razorvolumeconfiguration.h
    audioengine.h
)

set(UIS
    razorvolumeconfiguration.ui
)

if (PULSEAUDIO_FOUND)
    add_definitions(-DUSE_PULSEAUDIO)
    include_directories(${PULSEAUDIO_INCLUDE_DIR})
    set(HEADERS ${HEADERS} pulseaudioengine.h)
    set(SOURCES ${SOURCES} pulseaudioengine.cpp)
    set(MOCS ${MOCS} pulseaudioengine.h)
    LINK_LIBRARIES(${PLUGIN} ${PULSEAUDIO_LIBRARY})
endif (PULSEAUDIO_FOUND)

if (ALSA_FOUND)
    add_definitions(-DUSE_ALSA)
    set(HEADERS ${HEADERS} alsaengine.h alsadevice.h)
    set(SOURCES ${SOURCES} alsaengine.cpp alsadevice.cpp)
    set(MOCS ${MOCS} alsaengine.h alsadevice.h)
    LINK_LIBRARIES(${PLUGIN} ${ALSA_LIBRARIES})
endif (ALSA_FOUND)

set(LIBRARIES ${LIBRARIES} razorqxt )

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
