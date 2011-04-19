cmake_minimum_required( VERSION 2.6 )

set(PROJECT razor-panel )

set(QT_USE_QTXML 1)
set(QT_USE_QTDBUS 1)

set(HEADERS
    razorpanel.h
    razorpanel_p.h
    razorpanelplugin.h
    razorpanelplugin_p.h
    razorpanelapplication.h
    razorpanellayout.h
)

set(SOURCES
    main.cpp
    razorpanel.cpp
    razorpanelplugin.cpp
    razorpanelapplication.cpp
    razorpanellayout.cpp
)

set(MOCS
    razorpanel.h
    razorpanel_p.h
    razorpanelplugin.h
    razorpanelplugin_p.h
    razorpanelapplication.h
    razorpanellayout.h
)

set(LIBRARIES
    razorqt
)

# QRC files .................
set(RESOURCES

)

add_definitions(-DDEFULT_PANEL_PLUGINS=\"MainMenu,Quicklaunch,TaskBar,Tray,Clock\")

# Translations .............. 
set(TRANSLATIONS_DIR 
    ${CMAKE_INSTALL_PREFIX}/share/razor/${PROJECT}
)

add_definitions(-DTRANSLATIONS_DIR=\"${TRANSLATIONS_DIR}\")

file (GLOB CONFIG_FILES     resources/*.conf)

file (GLOB TS_FILES 
    translations/*.ts
)

############################################

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions (-Wall)
#    add_definitions (-Wall -pedantic)
endif()


# some system (rpm builds) setup LIB_SUFFIX for cmake. If there is no set, try to get it from system
IF (NOT LIB_SUFFIX)
        MESSAGE(STATUS "")
        MESSAGE(STATUS "LIB_SUFFIX variable is not defined. It will be autodetected now.")
        MESSAGE(STATUS "You can set it manually with -DLIB_SUFFIX=<value> (64 for example)")
        IF (NOT APPLE)
                # check 64 bit
                IF (CMAKE_SIZEOF_VOID_P EQUAL 4)
                        SET (LIB_SUFFIX "")
                ELSE (CMAKE_SIZEOF_VOID_P EQUAL 4)
                        SET (LIB_SUFFIX "64")
                        MESSAGE(STATUS " LIB_SUFFIX is set to '${LIB_SUFFIX}'")
                ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 4)
        ELSE (NOT APPLE)
                SET (LIB_SUFFIX "")
        ENDIF (NOT APPLE)
        # BSD does not use lib64 for 64bit libs
        IF (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
                message(STATUS "LIB_SUFFIX is disabled for *BSD: ${CMAKE_SYSTEM_NAME}")
                SET (LIB_SUFFIX "")
        ENDIF()
        #
        MESSAGE(STATUS "")
ENDIF (NOT LIB_SUFFIX)


set(PLUGIN_DESKTOPS_DIR
    ${CMAKE_INSTALL_PREFIX}/share/razor/${PROJECT}/plugins
)

add_definitions(-DPLUGIN_DESKTOPS_DIR=\"${PLUGIN_DESKTOPS_DIR}\")


#*******************************************
project(${PROJECT})

find_package(Qt4 REQUIRED)
#find_package ( X11 REQUIRED )

include(${QT_USE_FILE})
#qt4_wrap_cpp(MOC_SOURCES ${HEADERS})
qt4_wrap_cpp(MOC_SOURCES ${MOCS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})
qt4_add_translation(QM_FILES ${TS_FILES})

set (PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-panel/)
add_definitions(-DPLUGIN_DIR=\"${PLUGIN_DIR}\")
message(STATUS "Panel plugins location: ${PLUGIN_DIR}")

add_executable(${PROJECT} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES})
target_link_libraries(${PROJECT} ${LIBRARIES} ${QT_LIBRARIES})

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES   ${QM_FILES}        DESTINATION ${TRANSLATIONS_DIR})
install(FILES   ${CONFIG_FILES}    DESTINATION share/razor)
