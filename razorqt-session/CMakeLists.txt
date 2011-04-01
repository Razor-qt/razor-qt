project( razor-session )
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
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/librazorqt/ . src)

set ( razor-session_HDRS
	)

set ( razor-session_SRCS
	src/main.cpp
	src/razormodman.cpp
	)

set ( razor-session_MOCS
	src/razormodman.h
    src/sessiondbusadaptor.h
	)
QT4_WRAP_CPP(MOCS ${razor-session_MOCS})

add_executable ( razor-session ${razor-session_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( razor-session  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTXML_LIBRARY} ${X11_X11_LIB} ${CMAKE_BINARY_DIR}/librazorqt/librazorqt.so )
INSTALL(TARGETS razor-session RUNTIME DESTINATION bin)
