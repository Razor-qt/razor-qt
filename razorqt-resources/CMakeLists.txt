cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )

FILE(GLOB files "res/usrshare/*.*")



    IF (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
        message(STATUS "*BSD: ${CMAKE_SYSTEM_NAME} - Installing BSD-resources")
        install (DIRECTORY res/bsd/ DESTINATION share/razor)
    ELSE ()
	message(STATUS " Linux: ${CMAKE_SYSTEM_NAME} - Installing Linux-resources")
	install (DIRECTORY res/linux/ DESTINATION share/razor)
    ENDIF()







include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	)
