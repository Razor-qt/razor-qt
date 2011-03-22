project( razor-desktop )
cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( X11 REQUIRED )

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


set (DESKTOP_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-desktop/)
add_definitions(-DDESKTOP_PLUGIN_DIR=\"${DESKTOP_PLUGIN_DIR}\")
message(STATUS "Desktop plugins location: ${DESKTOP_PLUGIN_DIR}")


include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR} ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/librazorqt/
	.
	src)


set ( razor-desktop_SRCS
	src/main.cpp
	)

add_executable ( razor-desktop ${razor-desktop_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( razor-desktop  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTDBUS_LIBRARY}  ${X11_X11_LIB} ${CMAKE_BINARY_DIR}/librazorqt/librazorqt.so)
INSTALL(TARGETS razor-desktop RUNTIME DESTINATION bin)
install(DIRECTORY resources/ DESTINATION share/razor PATTERN ".svn" EXCLUDE)

add_subdirectory( desktop-wm_native )
add_subdirectory( desktop-razor )
