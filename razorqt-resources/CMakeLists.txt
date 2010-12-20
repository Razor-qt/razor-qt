cmake_minimum_required( VERSION 2.6 )

install(DIRECTORY config/ DESTINATION share/razor PATTERN ".svn" EXCLUDE)
# TODO/FIXME: install kdm/gdm files too

#
#IF (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
#    message(STATUS "*BSD: ${CMAKE_SYSTEM_NAME} - Installing BSD-resources")
#    install (DIRECTORY res/bsd/ DESTINATION share/razor PATTERN ".svn" EXCLUDE)
#ELSE ()
#	message(STATUS " Linux: ${CMAKE_SYSTEM_NAME} - Installing Linux-resources")
#	install (DIRECTORY res/linux/ DESTINATION share/razor PATTERN ".svn" EXCLUDE)
#ENDIF()


