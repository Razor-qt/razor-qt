# XDG standards expects system-wide configuration files in the /etc/xdg/razor location.
# Unfortunately QSettings we are using internally can be overriden in the Qt compilation
# time to use different path for system-wide configs. (for example configure ... -sysconfdir /etc/settings ...)
# This path can be found calling Qt4's qmake:
#   qmake -query QT_INSTALL_CONFIGURATION
#
# We check RAZOR_ETC_XDG_DIRECTORY for backward compatability
#    15.03.2013   A. Sokoloff

if (NOT DEFINED RAZOR_ETC_XDG_DIR AND NOT DEFINED RAZOR_ETC_XDG_DIRECTORY)
    if (NOT QT_QMAKE_EXECUTABLE)
        message(FATAL_ERROR "RAZOR_ETC_XDG_DIR: qmake not found or wrongly detected (inlude before qt configured?)")
    endif (NOT QT_QMAKE_EXECUTABLE)

    execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_CONFIGURATION
                        OUTPUT_VARIABLE RAZOR_ETC_XDG_DIR
                        OUTPUT_STRIP_TRAILING_WHITESPACE)

    message(STATUS "RAZOR_ETC_XDG_DIR autodetected as '${RAZOR_ETC_XDG_DIR}'")
    message(STATUS "You can set it manually with -DRAZOR_ETC_XDG_DIR=<value>")
    message(STATUS "")
#else ()
#    message(STATUS "RAZOR_ETC_XDG_DIR already set: ${RAZOR_ETC_XDG_DIR}")
endif ()

