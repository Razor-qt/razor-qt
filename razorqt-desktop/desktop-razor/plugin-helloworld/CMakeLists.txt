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
# Translations **********************************
include(RazorTranslate)
file (GLOB DESKTOP_FILES_IN resources/*.desktop.in)
razor_translate_desktop(DESKTOP_FILES ${DESKTOP_FILES_IN})
#************************************************

add_library(${PROJECT} SHARED ${HW_HEADERS} ${HW_SOURCES} ${HW_MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES} ${DESKTOP_FILES})
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )
add_dependencies(${PROJECT} razorqt)

install(TARGETS ${PROJECT} DESTINATION ${DESKTOP_PLUGIN_DIR})
install(FILES ${DESKTOP_FILES} DESTINATION share/razor/razor-desktop/)
install(FILES ${QM_FILES}      DESTINATION ${PLUGIN_SHARE_DIR})

