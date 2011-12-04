cmake_minimum_required(VERSION 2.6)

set(PLUGIN "razor")


set(HEADERS
  razorworkspace.h
  workspacemanager.h
  arrangeitem.h
  desktopbackgrounddialog.h
)

set(SOURCES
  razorworkspace.cpp
  workspacemanager.cpp
  arrangeitem.cpp
  desktopbackgrounddialog.cpp
)

set(MOCS
  razorworkspace.h
  workspacemanager.h
  arrangeitem.h
  desktopbackgrounddialog.h
)

set(UIS
    desktopbackgrounddialog.ui
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

qt4_wrap_cpp(MOC_SOURCES ${MOCS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})
qt4_add_translation(QM_FILES ${TS_FILES})
qt4_wrap_ui(UI_SOURCES ${UIS})

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions(-DPLUGIN_DESKTOP_FILES_DIR=\"${CMAKE_INSTALL_PREFIX}/share/razor/razor-desktop\")

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES} ${UI_SOURCES})
add_dependencies(${PROJECT} razorqt)
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${X11_X11_LIB} razorqt)

install(TARGETS ${PROJECT} DESTINATION ${DESKTOP_PLUGIN_DIR})

add_subdirectory( plugin-helloworld )
add_subdirectory( plugin-iconview )
add_subdirectory( plugin-analogclock )
