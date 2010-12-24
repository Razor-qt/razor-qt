project( razor-panel )
cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

# some system (rpm builds) setup LIB_SUFFIX for cmake. If there is no set, try to get it from system
IF (NOT LIB_SUFFIX)
	MESSAGE(STATUS "")
	MESSAGE(STATUS "LIB_SUFFIX variable is not defined. It will be autodetected now.")
	MESSAGE(STATUS "You can set it manually with -DLIB_SUFFIX=<value> (64 for example)")
	IF (NOT APPLE)
		# check 64 bit
		IF (CMAKE_SIZEOF_VOID_P EQUAL 4)
			SET (LIB_SUFFIX "")
		ELSE (CMAKE_SIZEOF_VOID_P EQUAL 4)
			SET (LIB_SUFFIX "64")
			MESSAGE(STATUS " LIB_SUFFIX is set to '${LIB_SUFFIX}'")
		ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 4)
	ELSE (NOT APPLE)
		SET (LIB_SUFFIX "")
	ENDIF (NOT APPLE)
	# BSD does not use lib64 for 64bit libs
	IF (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
		message(STATUS "LIB_SUFFIX is disabled for *BSD: ${CMAKE_SYSTEM_NAME}")
		SET (LIB_SUFFIX "")
	ENDIF()
	#
	MESSAGE(STATUS "")
ENDIF (NOT LIB_SUFFIX)


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

set (PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-panel/)
add_definitions(-DPLUGIN_DIR=\"${PLUGIN_DIR}\")
message(STATUS "Panel plugins location: ${PLUGIN_DIR}")

set ( razor-panel_HDRS
	src/defs.h
	src/razor.h
	src/razoraboutdlg.h
	src/razorgui.h
	src/razorversion.h
	src/razorplugin.h
	src/razorpluginmanager.h
	)

set ( razor-panel_SRCS
	src/main.cpp
	src/razor.cpp
	src/razorevent.cpp
	src/razoraboutdlg.cpp
	src/razorpluginmanager.cpp
	src/razorbar.cpp
	src/razorgui.cpp
	src/razorhandler.cpp
	)

set ( razor-panel_UIS
	src/aboutdlg.ui
	)
QT4_WRAP_UI(UIS ${razor-panel_UIS})

set ( razor-panel_MOCS
    src/razorplugin.h
	src/razorbar.h
	src/razorevent.h
	src/razorhandler.h
	)
QT4_WRAP_CPP(MOCS ${razor-panel_MOCS})

add_executable ( razor-panel ${razor-panel_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( razor-panel  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTDBUS_LIBRARY}  ${CMAKE_BINARY_DIR}/librazorqt/librazorqt.so )
INSTALL(TARGETS razor-panel RUNTIME DESTINATION bin)

#dev plugin
QT4_WRAP_CPP(devplugin_moc plugin-devplugin/razordevplugin.h)
add_library(razorpanel_devplugin SHARED ${devplugin_moc} plugin-devplugin/razordevplugin.cpp)
target_link_libraries(razorpanel_devplugin ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS razorpanel_devplugin DESTINATION ${PLUGIN_DIR})

QT4_WRAP_CPP(clock_moc plugin-clock/razorclock.h)
add_library(razorpanel_clock SHARED ${clock_moc} plugin-clock/razorclock.cpp)
target_link_libraries(razorpanel_clock ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS razorpanel_clock DESTINATION ${PLUGIN_DIR})

QT4_WRAP_CPP(deskswitch_moc plugin-deskswitch/razordeskswitch.h)
add_library(razorpanel_desktopswitcher SHARED ${deskswitch_moc} plugin-deskswitch/razordeskswitch.cpp)
target_link_libraries(razorpanel_desktopswitcher ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS razorpanel_desktopswitcher DESTINATION ${PLUGIN_DIR})

QT4_WRAP_CPP(logoutmenu_moc plugin-logoutmenu/razorlogoutmenu.h)
add_library(razorpanel_logoutmenu SHARED ${logoutmenu_moc} plugin-logoutmenu/razorlogoutmenu.cpp)
target_link_libraries(razorpanel_logoutmenu ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS razorpanel_logoutmenu DESTINATION ${PLUGIN_DIR})

QT4_WRAP_CPP(quicklaunch_moc plugin-quicklaunch/razorquicklaunch.h)
add_library(razorpanel_quicklaunch SHARED ${quicklaunch_moc} plugin-quicklaunch/razorquicklaunch.cpp)
target_link_libraries(razorpanel_quicklaunch ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS razorpanel_quicklaunch DESTINATION ${PLUGIN_DIR})

QT4_WRAP_CPP(spinbutton_moc plugin-spinbutton/razorspinbutton.h)
add_library(razorpanel_razorspinbutton SHARED ${spinbutton_moc} plugin-spinbutton/razorspinbutton.cpp)
target_link_libraries(razorpanel_razorspinbutton ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS razorpanel_razorspinbutton DESTINATION ${PLUGIN_DIR})

QT4_WRAP_CPP(tray_moc plugin-tray/razortray.h)
add_library(razorpanel_traybar SHARED ${tray_moc} plugin-tray/razortray.cpp)
target_link_libraries(razorpanel_traybar ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS razorpanel_traybar DESTINATION ${PLUGIN_DIR})

# to be replaced with alex's stuff
#QT4_WRAP_CPP(mainmenu_moc plugin-mainmenu/razormainmenu.h)
#add_library(razorpanel_mainmenu SHARED ${mainmenu_moc} plugin-mainmenu/razormainmenu.cpp)
#target_link_libraries(razorpanel_mainmenu ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
#install(TARGETS razorpanel_mainmenu DESTINATION ${PLUGIN_DIR})

QT4_WRAP_CPP(cmd_moc plugin-command/razorcmd.h)
add_library(razorpanel_cmd SHARED ${cmd_moc} plugin-command/razorcmd.cpp)
target_link_libraries(razorpanel_cmd ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS razorpanel_cmd DESTINATION ${PLUGIN_DIR})

QT4_WRAP_CPP(taskmanager_moc plugin-taskbar/razortask.h plugin-taskbar/razorbartask.h)
add_library(razorpanel_taskmanager SHARED ${taskmanager_moc} plugin-taskbar/razortask.cpp plugin-taskbar/razorbartask.cpp)
target_link_libraries(razorpanel_taskmanager ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS razorpanel_taskmanager DESTINATION ${PLUGIN_DIR})

QT4_WRAP_CPP(spacer_moc plugin-spacer/spacer.h)
add_library(razorpanel_spacer SHARED ${spacer_moc} plugin-spacer/spacer.cpp)
target_link_libraries(razorpanel_spacer ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS razorpanel_spacer DESTINATION ${PLUGIN_DIR})


add_subdirectory( plugin-battery )