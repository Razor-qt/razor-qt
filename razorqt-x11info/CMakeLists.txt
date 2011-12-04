project( razor-x11info )
cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( X11 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} 
    ${X11_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/
)

set ( razor-appswitcher_SRCS
	main.cpp
	)


add_executable ( razor-x11info ${razor-appswitcher_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( razor-x11info ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${X11_X11_LIB} razorqt )
add_dependencies(razor-x11info razorqt)
INSTALL(TARGETS razor-x11info RUNTIME DESTINATION bin)

