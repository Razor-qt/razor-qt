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

if ( NOT(${PULSEAUDIO_LIB} MATCHES "NOTFOUND") )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_PULSEAUDIO")
    set(HEADERS ${HEADERS} pulseaudioengine.h)
    set(SOURCES ${SOURCES} pulseaudioengine.cpp)
    set(MOCS ${MOCS} pulseaudioengine.h)
endif ( NOT(${PULSEAUDIO_LIB} MATCHES "NOTFOUND") )

if ( NOT(${ALSA_LIB} MATCHES "NOTFOUND") )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_ALSA")
    set(HEADERS ${HEADERS} alsaengine.h alsadevice.h)
    set(SOURCES ${SOURCES} alsaengine.cpp alsadevice.cpp)
    set(MOCS ${MOCS} alsaengine.h alsadevice.h)
endif ( NOT(${ALSA_LIB} MATCHES "NOTFOUND") )

set(LIBRARIES ${LIBRARIES} razorqxt )

LINK_LIBRARIES(${PLUGIN} ${PULSEAUDIO_LIB} ${ALSA_LIB})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
