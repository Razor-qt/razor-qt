# Install script for directory: /usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules

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

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/librazorqt/cmake_install.cmake")
  INCLUDE("/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/razorqt-session/cmake_install.cmake")
  INCLUDE("/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/razorqt-panel/cmake_install.cmake")
  INCLUDE("/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/razorqt-desktop/cmake_install.cmake")
  INCLUDE("/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/razorqt-resources/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/usr/home/vdop/projects/razor-release/razorqt-0.2-RC2/razorqt-modules/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
