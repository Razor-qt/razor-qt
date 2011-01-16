cmake_minimum_required(VERSION 2.6)

set(PLUGIN_NAME "mainmenu")


set(HEADERS
  razormainmenu.h
  menustyle.h
)

set(SOURCES
  razormainmenu.cpp
  menustyle.cpp
)

set(LIBRARIES
  razorqt
)

# QRC files .................
set(RESOURCES
)

set (CONFIG_FILES
  resources/mainmenu.conf
)

set(QT_USE_QTXML 1)
set(QT_USE_QTDBUS 1)


#*******************************************
set(PROJECT "razorpanel_${PLUGIN_NAME}")

project(${PROJECT})

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../../librazorqt
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if (NOT DEFINED PLUGIN_DIR)
  set (PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-panel/)
endif (NOT DEFINED PLUGIN_DIR)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
qt4_wrap_cpp(MOC_SOURCES ${HEADERS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})

#add_executable(${PROJECT} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES})
#target_link_libraries(${PROJECT} ${LIBRARIES} ${QT_LIBRARIES})

add_library(razorpanel_mainmenu SHARED ${MOC_SOURCES} ${SOURCES})
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS ${PROJECT} DESTINATION ${PLUGIN_DIR})
install(FILES ${CONFIG_FILES} DESTINATION share/razor)
