cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	)

# This allows to install and read the configs from non-standard locations
if (CMAKE_INSTALL_PREFIX)
    add_definitions(-DSHARE_DIR="\\"${CMAKE_INSTALL_PREFIX}/share/razor/\\"")
endif ()

add_subdirectory( librazorqt )
add_subdirectory( razorqt-session )
add_subdirectory( razorqt-panel )
add_subdirectory( razorqt-desktop )
add_subdirectory( razorqt-resources )



########### Add uninstall target ###############
CONFIGURE_FILE(
    "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")


