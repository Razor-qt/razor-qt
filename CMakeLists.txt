cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

set(RAZOR_VERSION 0.4.0)
add_definitions(-DRAZOR_VERSION=\"${RAZOR_VERSION}\")
message(STATUS "")
message(STATUS "Razor-qt version: ${RAZOR_VERSION}")
message(STATUS "")

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(RazorConfigRegister)


add_definitions ( -Wall )
find_package ( Qt4 4.6.0 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	)

find_package (UDev REQUIRED)

# This allows to install and read the configs from non-standard locations
if (CMAKE_INSTALL_PREFIX)
    add_definitions(-DSHARE_DIR=\"${CMAKE_INSTALL_PREFIX}/share/razor/\")
endif ()


# razor-config "modules"/desktop files directory
set(RAZOR_CONFIG_MODULES_DIR ${CMAKE_INSTALL_PREFIX}/share/razor/razor-config/)
add_definitions(-DRAZOR_CONFIG_MODULES_DIR=\"${RAZOR_CONFIG_MODULES_DIR}\")


# main build subdirs
add_subdirectory( librazorqt )
add_subdirectory( razorqt-session )
add_subdirectory( razorqt-panel )
add_subdirectory( razorqt-desktop )
add_subdirectory( razorqt-appswitcher )
add_subdirectory( razorqt-resources )
#add_subdirectory( razorqt-su )
add_subdirectory( razorqt-x11info )
add_subdirectory( razorqt-runner )
add_subdirectory( razorqt-config )


########### Add uninstall target ###############
CONFIGURE_FILE(
    "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")



########## Add make dist target ################
SET(CPACK_PACKAGE_VERSION "${RAZOR_VERSION}")
SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "razorqt-${CPACK_PACKAGE_VERSION}")
SET(CPACK_IGNORE_FILES "/CVS/;/\\\\.git\;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;refresh-copyright-and-license.pl")
SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})
INCLUDE(CPack)
# simulate autotools' "make dist"
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

