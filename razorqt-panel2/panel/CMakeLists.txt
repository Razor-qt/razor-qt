cmake_minimum_required( VERSION 2.6 )

set(PROJECT razor-panel2 )

set(QT_USE_QTXML 1)
set(QT_USE_QTDBUS 1)

set(HEADERS
    panel.h
    razorpanelplugin.h
    razorpluginmanager.h
)

set(SOURCES
    main.cpp
    panel.cpp
    razorpanelplugin.cpp
    razorpluginmanager.cpp
)

set(LIBRARIES
    razorqt
   # razorqtxdg
)

# QRC files .................
set(RESOURCES

)


############################################

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

if(CMAKE_COMPILER_IS_GNUCXX)
        add_definitions (-Wall -pedantic)
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




#*******************************************
project(${PROJECT})

find_package(Qt4 REQUIRED)
#find_package ( X11 REQUIRED )

include(${QT_USE_FILE})
qt4_wrap_cpp(MOC_SOURCES ${HEADERS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})

set (PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-panel/)
add_definitions(-DPLUGIN_DIR=\"${PLUGIN_DIR}\")
message(STATUS "Panel plugins location: ${PLUGIN_DIR}")

add_executable(${PROJECT} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES})
target_link_libraries(${PROJECT} ${LIBRARIES} ${QT_LIBRARIES})














# add_definitions ( -Wall )
# find_package ( Qt4 REQUIRED )
# find_package ( X11 REQUIRED )
# include ( ${QT_USE_FILE} )
# include_directories (
# 	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
# 	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR}
#        ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/librazorqt/
# 	.
# 	panel)

# set (PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-panel/)
# add_definitions(-DPLUGIN_DIR=\"${PLUGIN_DIR}\")
# message(STATUS "Panel plugins location: ${PLUGIN_DIR}")

# set ( razor-panel_HDRS
#         panel.h
#     )

# set ( razor-panel_SRCS
#         panel.cpp
#     )


# set ( razor-panel_MOCS
#     )

# QT4_WRAP_CPP(MOCS ${razor-panel_MOCS})

# add_executable ( razor-panel ${razor-panel_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
# target_link_libraries ( razor-panel  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTDBUS_LIBRARY}  ${CMAKE_BINARY_DIR}/librazorqt/librazorqt.so )
#INSTALL(TARGETS razor-panel RUNTIME DESTINATION bin)

