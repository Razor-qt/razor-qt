cmake_minimum_required(VERSION 2.6)

set(PLUGIN "razor")


set(HEADERS
  razorworkspace.h
  workspacemanager.h
)

set(SOURCES
  razorworkspace.cpp
  workspacemanager.cpp
)

set(MOCS
  razorworkspace.h
  workspacemanager.h
)

# QRC files
set(RESOURCES
)


file (GLOB CONFIG_FILES
  resources/*.conf
)


#*******************************************
set(PROJECT "desktop-${PLUGIN}")

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
qt4_wrap_cpp(MOC_SOURCES ${MOCS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})
qt4_add_translation(QM_FILES ${TS_FILES})

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES})
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${X11_X11_LIB} ${CMAKE_BINARY_DIR}/librazorqt/librazorqt.so)

install(TARGETS ${PROJECT} DESTINATION ${DESKTOP_PLUGIN_DIR})
install(FILES ${CONFIG_FILES} DESTINATION share/razor)
install(FILES ${QM_FILES} DESTINATION ${TRANSLATIONS_DIR}2)


add_subdirectory( plugin-helloworld )
add_subdirectory( plugin-iconview )
