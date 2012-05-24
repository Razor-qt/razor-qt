cmake_minimum_required(VERSION 2.6)
OPTION(ENABLE_LIGHTDM_GREETER "Build lightdm greeter" ON)

if (ENABLE_LIGHTDM_GREETER)

set(PROJECT razor-lightdm-greeter)
project($PROJECT)


# This HACK is here to allow greeter to be built as a standalone binary...
# TODO/FIXME: check all modules to be built standalone...
if (CMAKE_SOURCE_DIR STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    message(WARNING "This is a standalone build of razor's lightdm greeter.")
    # This allows to install and read the configs from non-standard locations
    add_definitions(-DSHARE_DIR=\"${CMAKE_INSTALL_PREFIX}/share/razor/\")
else ()
    message(STATUS "LightDM Greeter is a part of razor-qt build")
endif()


find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )

# check for new version
include(FindPkgConfig)
message(STATUS "Checking for lightdm-qt...")
pkg_check_modules(LIGHTDM_QT liblightdm-qt-2)
# or lightdm v1 if the version 2 is not found
if (NOT LIGHTDM_QT_FOUND)
    message(STATUS "lightdm2 for Qt is not found. Trying to find v1")
    pkg_check_modules(LIGHTDM_QT liblightdm-qt-1)
endif()
# decision for version. It needs to be separated as the cmake can
# be run more times. And the LIGHTDM_QT_FOUND is cached so it's
# marked as v2 for every cmake run (except 1st one) in this case.
if (LIGHTDM_QT_VERSION VERSION_LESS "1.1.0")
    add_definitions(-DUSING_LIGHTDM_QT_1)
endif()

if(NOT LIGHTDM_QT_INCLUDE_DIRS)
    message(STATUS "Qt bindings for liblightdm not found. Razor-lightdm-greeter won't be built")
    message(STATUS "Hint: On debian or debian-derived distributions you could probably do: sudo apt-get install liblightdm-qt-1-dev")
    message(STATUS "      On suse or opensuse distribution: lightdm-qt-devel")
elseif(NOT LIGHTDM_QT_LIBRARIES)
    message(STATUS "lightdm-qt library not found. Razor-lightdm-greeter won't be built")
    message(STATUS "Hint: On debian or debian-derived distributions the package would be something like: liblightdm-qt-1-0")
    message(STATUS "      On suse or opensuse distribution: liblightdm-qt-1-0")
else()

    ############### Patch #############################################################
    # liblightdm-qt-1.pc (pkg-config file) erroneously reports includedir as
    # /usr/include/lightdm-qt-0 and the library as lightdm-qt-0. So we have to patch
    # Bug filed: https://bugs.launchpad.net/ubuntu/+source/lightdm/+bug/943914

    foreach(INCLUDEDIR ${LIGHTDM_QT_INCLUDE_DIRS})
        string(REPLACE "-qt-0" "-qt-1" INCLUDEDIR ${INCLUDEDIR})
        list(APPEND TMP1 ${INCLUDEDIR})
    endforeach()
    set(LIGHTDM_QT_INCLUDE_DIRS ${TMP1})

    foreach(LIBRARY ${LIGHTDM_QT_LIBRARIES})
        string(REPLACE "-qt-0" "-qt-1" LIBRARY ${LIBRARY})
        list(APPEND TMP2 ${LIBRARY})
    endforeach()
    set(LIGHTDM_QT_LIBRARIES ${TMP2})
    ############## End patch #########################################################

    include_directories (
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}
            ${CMAKE_SOURCE_DIR}/libraries/
            ${CMAKE_SOURCE_DIR}/libraries/razorqt
            ${LIGHTDM_QT_INCLUDE_DIRS}
            )

    set ( razor-lightdm-greeter_SRCS
          main.cpp
          mainwindow.cpp
          loginform.cpp
        )

    set ( razor-lightdm-greeter_UIS
          loginform.ui
        )
    QT4_WRAP_UI(UIS ${razor-lightdm-greeter_UIS})

    set ( razor-lightdm-greeter_RSCS )
    QT4_ADD_RESOURCES(RSCS ${razor-lightdm-greeter_RSCS})

    set ( razor-lightdm-greeter_MOCS
          mainwindow.h
          loginform.h
        )
    QT4_WRAP_CPP(MOCS ${razor-lightdm-greeter_MOCS})

    if(CMAKE_BUILD_TYPE MATCHES [Dd]ebug)
        configure_file(test/test.sh test/test.sh)
        configure_file(test/lightdm-razor-greeter.desktop test/lightdm-razor-greeter.desktop)
        configure_file(test/lightdm.conf test/lightdm.conf)
    endif()

    link_directories( ${LIGHTDM_QT_LIBRARY_DIRS} )
    add_executable ( razor-lightdm-greeter ${razor-lightdm-greeter_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
    target_link_libraries ( razor-lightdm-greeter  razorqt ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${LIGHTDM_QT_LIBRARIES} )

    file(GLOB DESKTOP_FILES resources/*.desktop)

    install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
    install(FILES   ${DESKTOP_FILES}   DESTINATION /usr/share/xgreeters)
endif()

endif (ENABLE_LIGHTDM_GREETER)
