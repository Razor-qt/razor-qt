cmake_minimum_required( VERSION 2.6 )

install(DIRECTORY config/ DESTINATION share/razor PATTERN ".svn" EXCLUDE)
install(DIRECTORY graphics DESTINATION share/razor/ PATTERN ".svn" EXCLUDE)

# TODO/FIXME: install kdm/gdm files too
# this file should be enough but it needs testing...
set(SESSION_FILES
    sys/razor.desktop
#    sys/razor-eggwm.desktop
    sys/razor-openbox.desktop
    sys/razor-kwin.desktop
    sys/razor-metacity.desktop
)

MACRO(INSTALL_SESSION_FILES directory)
#    if(EXISTS ${directory})
        install(FILES ${SESSION_FILES} DESTINATION ${directory})
#    endif()
ENDMACRO(INSTALL_SESSION_FILES)

#INSTALL_SESSION_FILES("/etc/X11/sessions")
#INSTALL_SESSION_FILES("/usr/share/sessions")
INSTALL_SESSION_FILES("/usr/share/xsessions")
INSTALL_SESSION_FILES("/usr/share/apps/kdm/sessions")
#INSTALL_SESSION_FILES("/usr/local/share/apps/kdm/sessions")
#INSTALL_SESSION_FILES("/usr/share/config/kdm/sessions")
#INSTALL_SESSION_FILES("/usr/local/share/config/kdm/sessions")
#
#IF (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
#    message(STATUS "*BSD: ${CMAKE_SYSTEM_NAME} - Installing BSD-resources")
#    install (DIRECTORY res/bsd/ DESTINATION share/razor PATTERN ".svn" EXCLUDE)
#ELSE ()
#	message(STATUS " Linux: ${CMAKE_SYSTEM_NAME} - Installing Linux-resources")
#	install (DIRECTORY res/linux/ DESTINATION share/razor PATTERN ".svn" EXCLUDE)
#ENDIF()

install(DIRECTORY autostart/ DESTINATION /etc/xdg/autostart)

add_subdirectory( themes )
add_subdirectory( application-menu )
