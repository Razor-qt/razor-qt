cmake_minimum_required(VERSION 2.6)

set(PLUGIN "razor")


set(HEADERS
  razorworkspace.h
  workspacemanager.h
  arrangeitem.h
)

set(SOURCES
  razorworkspace.cpp
  workspacemanager.cpp
  arrangeitem.cpp
)

set(MOCS
  razorworkspace.h
  workspacemanager.h
  arrangeitem.h
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

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES})
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${X11_X11_LIB} ${CMAKE_BINARY_DIR}/librazorqt/librazorqt.so)

install(TARGETS ${PROJECT} DESTINATION ${DESKTOP_PLUGIN_DIR})

add_subdirectory( plugin-helloworld )
add_subdirectory( plugin-iconview )
