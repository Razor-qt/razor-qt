project( razor-desktop )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( X11 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR} ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/librazorqt/
	.
	src)
set ( razor-desktop_HDRS
	src/razor.h
	)

set ( razor-desktop_SRCS
	src/razordeskicon.cpp
	src/main.cpp
	src/razordeskman.cpp
	src/razordeskmanl.cpp
	src/razorworkspace.cpp
	src/razor.cpp
	)

set ( razor-desktop_MOCS
	src/razordeskicon.h
	src/razordeskmanl.h
	src/razordeskman.h
	src/razorworkspace.h
	)
QT4_WRAP_CPP(MOCS ${razor-desktop_MOCS})

add_executable ( razor-desktop ${razor-desktop_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( razor-desktop  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTDBUS_LIBRARY}  ${X11_X11_LIB} ${CMAKE_SOURCE_DIR}/librazorqt/librazorqt.so)
INSTALL(TARGETS razor-desktop RUNTIME DESTINATION bin) 
