project( razor-desktop )
cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( X11 REQUIRED )

include(RazorLibSuffix)

set (DESKTOP_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-desktop/)
add_definitions(-DDESKTOP_PLUGIN_DIR=\"${DESKTOP_PLUGIN_DIR}\")
message(STATUS "Desktop plugins location: ${DESKTOP_PLUGIN_DIR}")


include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR} ${X11_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/
	src)


set ( razor-desktop_SRCS
	src/main.cpp
	)

add_executable ( razor-desktop ${razor-desktop_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
add_dependencies( razor-desktop razorqt)
target_link_libraries ( razor-desktop  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTDBUS_LIBRARY}  ${X11_X11_LIB} razorqt qtxdg)
INSTALL(TARGETS razor-desktop RUNTIME DESTINATION bin)
install(DIRECTORY resources/ DESTINATION share/razor PATTERN ".svn" EXCLUDE)

add_subdirectory(config)
add_subdirectory( desktop-wm_native )
add_subdirectory( desktop-razor )
