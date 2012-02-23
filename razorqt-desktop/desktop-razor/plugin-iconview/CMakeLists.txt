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

# Translations **********************************
include(RazorTranslate)
file (GLOB DESKTOP_FILES_IN resources/*.desktop.in)
razor_translate_desktop(DESKTOP_FILES ${DESKTOP_FILES_IN})
#************************************************

add_library(${PROJECT} MODULE ${IV_HEADERS} ${IV_SOURCES} ${IV_MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES} ${DESKTOP_FILES})
add_dependencies(${PROJECT} razorqt)
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )

install(TARGETS ${PROJECT} DESTINATION ${DESKTOP_PLUGIN_DIR})
install(FILES ${DESKTOP_FILES} DESTINATION share/razor/razor-desktop/)
install(FILES ${QM_FILES}      DESTINATION ${PLUGIN_SHARE_DIR})
