project( razor-desktop )
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
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR}
    ${X11_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/librazorqt/
	src
    src/qxt)

set ( razor-appswitcher_HDRS
	src/appswitcher.h
    src/qxt/qxtglobal.h
    src/qxt/qxtglobalshortcut.h
    src/qxt/qxtglobalshortcut_p.h
	)

set ( razor-appswitcher_SRCS
	src/main.cpp
	src/appswitcher.cpp
    src/qxt/qxtglobal.cpp
    src/qxt/qxtglobalshortcut.cpp
    src/qxt/qxtglobalshortcut_x11.cpp
	)

QT4_WRAP_CPP(MOCS ${razor-appswitcher_HDRS})
QT4_WRAP_UI(UIS src/appswitcher.ui)

SET (exec razor-appswitcher)

add_executable ( ${exec} ${razor-appswitcher_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( ${exec}  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTDBUS_LIBRARY} ${X11_X11_LIB} ${CMAKE_BINARY_DIR}/librazorqt/librazorqt.so)
INSTALL(TARGETS ${exec} RUNTIME DESTINATION bin)
