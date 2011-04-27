cmake_minimum_required( VERSION 2.6 )

install(DIRECTORY config/ DESTINATION share/razor PATTERN ".svn" EXCLUDE)
# TODO/FIXME: install kdm/gdm files too
# this file should be enough but it needs testing...
install(FILES sys/razor.desktop sys/razor-eggwm.desktop sys/razor-openbox.desktop DESTINATION share/xsessions)

#
#IF (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
#    message(STATUS "*BSD: ${CMAKE_SYSTEM_NAME} - Installing BSD-resources")
#    install (DIRECTORY res/bsd/ DESTINATION share/razor PATTERN ".svn" EXCLUDE)
#ELSE ()
#	message(STATUS " Linux: ${CMAKE_SYSTEM_NAME} - Installing Linux-resources")
#	install (DIRECTORY res/linux/ DESTINATION share/razor PATTERN ".svn" EXCLUDE)
#ENDIF()

add_subdirectory( themes )
