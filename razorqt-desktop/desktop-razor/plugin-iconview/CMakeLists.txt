cmake_minimum_required(VERSION 2.6)

set(PLUGIN "iconview")


set(IV_HEADERS
  iconview.h
  iconscene.h
  desktopicon.h
)

set(IV_SOURCES
  iconview.cpp
  iconscene.cpp
  desktopicon.cpp
)

set(IV_MOCS
  iconscene.h
  desktopicon.h
)

#*******************************************
set(PROJECT "${PLUGIN}")

project(${PROJECT})


include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}
)

qt4_wrap_cpp(IV_MOC_SOURCES ${IV_MOCS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})
qt4_add_translation(QM_FILES ${TS_FILES})

add_library(${PROJECT} SHARED ${IV_HEADERS} ${IV_SOURCES} ${IV_MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES})
add_dependencies(${PROJECT} razorqt)
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )

install(TARGETS ${PROJECT} DESTINATION ${DESKTOP_PLUGIN_DIR})
install(DIRECTORY resources/ DESTINATION share/razor/razor-desktop/ PATTERN ".svn" EXCLUDE)
