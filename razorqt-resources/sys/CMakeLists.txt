cmake_minimum_required( VERSION 2.6 )

# TODO/FIXME: install kdm/gdm files too
# this file should be enough but it needs testing...
file(GLOB SESSION_FILES_IN *.desktop.in)


# Translations **********************************
include(RazorTranslate)
razor_translate_desktop(SESSION_FILES
    SOURCES
        ${SESSION_FILES_IN}
)
add_custom_target(sys_desktop_files ALL DEPENDS ${SESSION_FILES})
#************************************************

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
