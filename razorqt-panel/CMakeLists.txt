project( razor-panel )
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
        ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/librazorqt/
	.
	src)
set ( razor-panel_HDRS
	src/defs.h
	src/razor.h
	src/razorclock.h
	src/razoraboutdlg.h
	src/razorclock.h
	src/razorgui.h
	src/razorversion.h
	src/razortask.h
	src/razorplugin.h
	src/razorpluginmanager.h
	src/razortray.h
	)

set ( razor-panel_SRCS
	src/main.cpp
	src/razor.cpp
	src/razorevent.cpp
	src/razorbartask.cpp
	src/razorlogoutmenu.cpp
	src/razoraboutdlg.cpp
	src/razorpluginmanager.cpp
	src/razorplugin.h
	src/razorclock.cpp
	src/razorbar.cpp
	src/razorcmd.cpp
	src/razortray.cpp
	src/razorgui.cpp
	src/razorspinbutton.cpp
	src/razordeskswitch.cpp
	src/razorhandler.cpp
	src/razormainmenu.cpp
	src/razortask.cpp
	)

set ( razor-panel_UIS
	src/aboutdlg.ui
	)
QT4_WRAP_UI(UIS ${razor-panel_UIS})

set ( razor-panel_MOCS
	src/razorbar.h
	src/razorevent.h
	src/razorhandler.h
	src/razormainmenu.h
	src/razorlogoutmenu.h
	src/razorclock.h
	src/razorcmd.h
	src/razordeskswitch.h
	src/razorspinbutton.h
	src/razorbartask.h
 	src/razortray.h
	)
QT4_WRAP_CPP(MOCS ${razor-panel_MOCS})

add_executable ( razor-panel ${razor-panel_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( razor-panel  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTDBUS_LIBRARY}  ${CMAKE_BINARY_DIR}/librazorqt/librazorqt.so )
INSTALL(TARGETS razor-panel RUNTIME DESTINATION bin) 
