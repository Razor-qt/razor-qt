cmake_minimum_required(VERSION 2.6)

set(PLUGIN "helloworld")


set(HW_HEADERS
  helloworld.h
)

set(HW_SOURCES
  helloworld.cpp
)

set(HW_MOCS

)

#*******************************************
set(PROJECT "${PLUGIN}")

project(${PROJECT})

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}
)

qt4_wrap_cpp(HW_MOC_SOURCES ${HW_MOCS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})
qt4_add_translation(QM_FILES ${TS_FILES})

add_library(${PROJECT} SHARED ${HW_HEADERS} ${HW_SOURCES} ${HW_MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES})
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )
add_dependencies(${PROJECT} razorqt)

install(TARGETS ${PROJECT} DESTINATION ${DESKTOP_PLUGIN_DIR})
install(DIRECTORY resources/ DESTINATION share/razor/razor-desktop/ PATTERN ".svn" EXCLUDE)
