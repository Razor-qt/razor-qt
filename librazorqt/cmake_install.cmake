# Install script for directory: /usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/librazorqt

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librazorqt.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librazorqt.so")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librazorqt.so"
         RPATH "")
  ENDIF()
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/librazorqt/librazorqt.so")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librazorqt.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librazorqt.so")
    FILE(RPATH_REMOVE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librazorqt.so")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librazorqt.so")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/razorqt" TYPE FILE FILES
    "/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/librazorqt/razorqt/defs.h"
    "/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/librazorqt/razorqt/readsettings.h"
    "/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/librazorqt/razorqt/xdgautostart.h"
    "/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/librazorqt/razorqt/xdgdesktopfile.h"
    "/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/librazorqt/razorqt/xdgiconthemefile.h"
    "/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/librazorqt/razorqt/xdgiconthememanager.h"
    "/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/librazorqt/razorqt/xdgmenucross.h"
    "/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/librazorqt/razorqt/xfitman.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

