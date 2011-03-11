cmake_minimum_required(VERSION 2.6)

set(PLUGIN "clock2")


set(HEADERS
  razorclock.h
)

set(SOURCES
  razorclock.cpp
)

set(MOCS
  razorclock.h
)

#set(LIBRARIES
#  razorqt
#)

# QRC files .................
set(RESOURCES
)


file (GLOB CONFIG_FILES
  resources/*.conf
)

#set(QT_USE_QTXML 1)
#set(QT_USE_QTDBUS 1)

#*******************************************
set(PROJECT "razorpanel_${PLUGIN}")

project(${PROJECT})

# Translations .............. 
set(TRANSLATIONS_DIR 
    ${CMAKE_INSTALL_PREFIX}/share/razor/razor-panel
)

add_definitions(-DTRANSLATIONS_DIR=\"${TRANSLATIONS_DIR}\")

file (GLOB TS_FILES 
    translations/*.ts
)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../../librazorqt
    ${CMAKE_CURRENT_SOURCE_DIR}/panel
)

if (NOT DEFINED PLUGIN_DIR)
  set (PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-panel/)
endif (NOT DEFINED PLUGIN_DIR)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
qt4_wrap_cpp(MOC_SOURCES ${MOCS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})
qt4_add_translation(QM_FILES ${TS_FILES})

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES})
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS ${PROJECT} DESTINATION ${PLUGIN_DIR})
install(FILES ${CONFIG_FILES} DESTINATION share/razor)
install(FILES ${QM_FILES} DESTINATION ${TRANSLATIONS_DIR}2)


