cmake_minimum_required(VERSION 2.6)
OPTION(ENABLE_LIGHTDM_GREETER "Build lightdm greeter" ON)

if (ENABLE_LIGHTDM_GREETER)

    set(PROJECT razor-lightdm-greeter)
    project(${PROJECT})

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

    if(NOT LIGHTDM_QT_INCLUDE_DIRS)
        message(STATUS "Qt bindings for liblightdm not found. Razor-lightdm-greeter won't be built")
        message(STATUS "Hint: On debian or debian-derived distributions you could probably do: sudo apt-get install liblightdm-qt-1-dev")
        message(STATUS "      On suse or opensuse distribution: lightdm-qt-devel")
    elseif(NOT LIGHTDM_QT_LIBRARIES)
        message(STATUS "lightdm-qt library not found. Razor-lightdm-greeter won't be built")
        message(STATUS "Hint: On debian or debian-derived distributions the package would be something like: liblightdm-qt-1-0")
        message(STATUS "      On suse or opensuse distribution: liblightdm-qt-1-0")
    else()

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
            logindata.cpp
            )

        set ( razor-lightdm-greeter_UIS
            loginform.ui
            )
        QT4_WRAP_UI(UIS ${razor-lightdm-greeter_UIS})

        set ( razor-lightdm-greeter_HDRS
            mainwindow.h
            loginform.h
            logindata.h
            )

        set ( razor-lightdm-greeter_MOCS
            mainwindow.h
            loginform.h
            logindata.h
            )

        QT4_WRAP_CPP(MOCS ${razor-lightdm-greeter_MOCS})

        if(CMAKE_BUILD_TYPE MATCHES [Dd]ebug)
            configure_file(test/test.sh test/test.sh)
            configure_file(test/lightdm-razor-greeter.desktop test/lightdm-razor-greeter.desktop)
            configure_file(test/lightdm.conf test/lightdm.conf)
        endif()

        # Translations **********************************
        include(RazorTranslate)
        razor_translate_ts( QM_FILES
                        SOURCES
                            ${razor-lightdm-greeter_HDRS}
                            ${razor-lightdm-greeter_SRCS}
                            ${razor-lightdm-greeter_UIS}
                        )
        #************************************************

        link_directories( ${LIGHTDM_QT_LIBRARY_DIRS} )
        add_executable ( razor-lightdm-greeter
                            ${razor-lightdm-greeter_SRCS}
                            ${razor-lightdm-greeter_HDRS}
                            ${razor-lightdm-greeter_UIS}
                            ${QM_FILES}
                            ${MOCS}
                    )
        target_link_libraries ( razor-lightdm-greeter  razorqt qtxdg ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${LIGHTDM_QT_LIBRARIES} )

        file(GLOB DESKTOP_FILES resources/*.desktop)
        file(GLOB CONFIG_FILES resources/*.conf)

        install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
        install(FILES   ${DESKTOP_FILES}   DESTINATION /usr/share/xgreeters)

        if (BACKUP_CONFIGS)
            # Use execute_process with 'cp' rather than cmake's install as I couldn't figure out how to make
            # install create a backup of destination.
            install(CODE "execute_process(COMMAND cp -v -u --backup=numbered \"${CONFIG_FILES}\" /etc/lightdm)")
        else()
            install(FILES ${CONFIG_FILES} DESTINATION /etc/lightdm)
        endif()

    endif()

endif (ENABLE_LIGHTDM_GREETER)
