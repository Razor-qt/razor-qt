project( razorqt )
cmake_minimum_required( VERSION 2.6 )

set (MAJOR_VERSION 0)
set (MINOR_VERSION 1)
set (PATCH_VERSION 0)

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
	.
	razorqt)
set ( razorqt_HDRS
	razorqt/defs.h
	razorqt/razoraboutdlg.h
	razorqt/razordevman.h
	razorqt/razorhal.h
	razorqt/readsettings.h
	razorqt/xdgautostart.h
	razorqt/xdgenv.h
	razorqt/xdgiconthemefile.h
	razorqt/xdgiconthememanager.h
	razorqt/xfitman.h
	razorqt/domhelper.h
	razorqt/xdgdesktopfile.h
	razorqt/xdgmenu.h
	)

set ( razorqt_SRCS
	razorqt/razoraboutdlg.cpp
	razorqt/razordevman.cpp
	razorqt/razorhal.cpp
	razorqt/readsettings.cpp
	razorqt/xdgautostart.cpp
	razorqt/xdgdesktopfile.cpp
	razorqt/xdgiconthemefile.cpp
	razorqt/xdgiconthememanager.cpp
	razorqt/xdgmanager.cpp
	razorqt/xdgmenu.cpp
	razorqt/xfitman.cpp
	razorqt/domhelper.cpp
	razorqt/xdgmenuapplinkprocessor.cpp
	razorqt/xdgmenureader.cpp
	razorqt/xdgmenurules.cpp

	)

set ( razorqt_MOCS
	razorqt/razoraboutdlg.h
	razorqt/razordevman.h
	razorqt/razorhal.h
	razorqt/xdgmanager.h
	razorqt/xdgmenu.h
	razorqt/readsettings.h
	razorqt/xdgdesktopfile.h
	razorqt/domhelper.h
	razorqt/xdgmenuapplinkprocessor.h
	razorqt/xdgmenureader.h
	razorqt/xdgmenurules.h
	)

QT4_WRAP_CPP(MOCS ${razorqt_MOCS})


set ( razorqt_UIS
        razorqt/aboutdlg.ui
        )

QT4_WRAP_UI(UIS ${razorqt_UIS})


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


add_library ( razorqt SHARED ${razorqt_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( razorqt  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTDBUS_LIBRARY}  ${X11_X11_LIB})
set_target_properties(razorqt PROPERTIES 
  VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION} 
  SOVERSION ${MAJOR_VERSION}
)


message(STATUS ${CMAKE_INSTALL_PREFIX})

install(TARGETS razorqt DESTINATION lib${LIB_SUFFIX})
install(FILES ${razorqt_HDRS} DESTINATION include/razorqt)
