cmake_minimum_required(VERSION 2.6)

set(PLUGIN "iconview")


set(IV_HEADERS
  iconview.h
)

set(IV_SOURCES
  iconview.cpp
  iconscene.cpp
  desktopicon.cpp
)

set(IV_MOCS
  iconview.h
)

# QRC files
set(RESOURCES
)


file (GLOB CONFIG_FILES
  resources/*.conf
)


#*******************************************
set(PROJECT "desktop-razor-${PLUGIN}")

project(${PROJECT})

# Translations
set(TRANSLATIONS_DIR 
    ${CMAKE_INSTALL_PREFIX}/share/razor/razor-panel
)

add_definitions(-DTRANSLATIONS_DIR=\"${TRANSLATIONS_DIR}\")

file (GLOB TS_FILES 
    translations/*.ts
)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../../librazorqt
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
qt4_wrap_cpp(IV_MOC_SOURCES ${IV_MOCS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})
qt4_add_translation(QM_FILES ${TS_FILES})

add_library(${PROJECT} SHARED ${IV_HEADERS} ${IV_SOURCES} ${IV_MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES})
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${CMAKE_CURRENT_BINARY_DIR}/../libdesktop-razor.so)

install(TARGETS ${PROJECT} DESTINATION ${DESKTOP_PLUGIN_DIR})
install(FILES ${CONFIG_FILES} DESTINATION share/razor)
install(FILES ${QM_FILES} DESTINATION ${TRANSLATIONS_DIR}2)


