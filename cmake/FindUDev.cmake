# razor-de: Configure libudev environment
#
# UDEV_FOUND - system has a libudev
# UDEV_INCLUDE_DIR - where to find header files
# UDEV_LIBRARIES - the libraries to link against udev
#
# copyright (c) 2011 Petr Vanek <petr@scribus.info>
# Redistribution and use is allowed according to the terms of the BSD license.
#

FIND_PATH(
    UDEV_INCLUDE_DIR
    libudev.h
    /usr/include
    /usr/local/include
    ${UDEV_PATH_INCLUDES}
)

FIND_LIBRARY(
    UDEV_LIBRARIES
    NAMES udev libudev
    PATHS
        /usr/lib${LIB_SUFFIX}
        /usr/local/lib${LIB_SUFFIX}
        ${UDEV_PATH_LIB}
)

IF (UDEV_LIBRARIES AND UDEV_INCLUDE_DIR)
    SET(UDEV_FOUND "YES")
ENDIF (UDEV_LIBRARIES AND UDEV_INCLUDE_DIR)

IF (UDEV_FOUND)
    MESSAGE(STATUS "Found UDev: ${UDEV_LIBRARIES}")
    MESSAGE(STATUS "   include: ${UDEV_INCLUDE_DIR}")
ELSE (UDEV_FOUND)
    MESSAGE(STATUS "UDev not found.")
    MESSAGE(STATUS "UDev: You can specify includes: -DUDEV_PATH_INCLUDES=/opt/udev/include")
    MESSAGE(STATUS "      currently found includes: ${UDEV_INCLUDE_DIR}")
    MESSAGE(STATUS "UDev: You can specify libs: -DUDEV_PATH_LIB=/opt/udev/lib")
    MESSAGE(STATUS "      currently found libs: ${UDEV_LIBRARIES}")
    IF (UDev_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find UDev library")
    ENDIF (UDev_FIND_REQUIRED)
ENDIF (UDEV_FOUND)
