# XDG standards expects system-wide configuration files in the /etc/xdg/razor location.
# Unfortunately QSettings we are using internally can be overriden in the Qt compilation
# time to use different path for system-wide configs. (for example configure ... -sysconfdir /etc/settings ...)
# This path can be found calling Qt4's qmake:
#   qmake -query QT_INSTALL_CONFIGURATION
#
if (NOT DEFINED RAZOR_ETC_XDG_DIRECTORY)
    message(STATUS "*********************************************************************")
    message(STATUS "RAZOR_ETC_XDG_DIRECTORY will be autodetected now")
    message(STATUS "You can set it manually with -DRAZOR_ETC_XDG_DIRECTORY=<value>")

    if (NOT QT_QMAKE_EXECUTABLE)
        message(FATAL_ERROR "RAZOR_ETC_XDG_DIRECTORY: qmake not found or wrongly detected (inlude before qt configured?)")
    endif (NOT QT_QMAKE_EXECUTABLE)

    execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_CONFIGURATION
	                    OUTPUT_VARIABLE RAZOR_ETC_XDG_DIRECTORY
			    OUTPUT_STRIP_TRAILING_WHITESPACE)

    message(STATUS "RAZOR_ETC_XDG_DIRECTORY autodetected as '${RAZOR_ETC_XDG_DIRECTORY}'")
    message(STATUS "*********************************************************************")
else ()
    message(STATUS "RAZOR_ETC_XDG_DIRECTORY already set: ${RAZOR_ETC_XDG_DIRECTORY}")
endif ()

