cmake_minimum_required(VERSION 2.6)

set(PLUGIN "wm_native")


set(HEADERS
  razordeskicon.h
  razordeskman.h
)

set(SOURCES
  razordeskicon.cpp
  razordeskman.cpp
)

set(MOCS
  razordeskicon.h
  razordeskman.h
)

# QRC files
set(RESOURCES
)


set(PROJECT "desktop-${PLUGIN}")

project(${PROJECT})

# Translations
set(TRANSLATIONS_DIR 
    ${CMAKE_INSTALL_PREFIX}/share/razor/razor-panel
)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

qt4_wrap_cpp(MOC_SOURCES ${MOCS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})
qt4_add_translation(QM_FILES ${TS_FILES})

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES})
add_dependencies(${PROJECT} razorqt)
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${X11_X11_LIB} razorqt)

install(TARGETS ${PROJECT} DESTINATION ${DESKTOP_PLUGIN_DIR})

