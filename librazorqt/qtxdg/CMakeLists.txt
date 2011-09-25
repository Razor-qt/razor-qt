project(libqtxdg)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set (MAJOR_VERSION 0)
set (MINOR_VERSION 1)
set (PATCH_VERSION 0)



add_definitions(-Wall)
find_package(Qt4 REQUIRED)
find_package(X11 REQUIRED)
find_package(LibMagic REQUIRED)


include ( ${QT_USE_FILE} )
#include_directories (
#	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
#	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR}
#	${X11_INCLUDE_DIR}
#	${LIBMAGIC_INCLUDE_DIR}
#	.
#	razorqt
#    librazor-upower
#    qxt
#)

set(QTXDG_PUBLIC_HDRS
    xdgaction.h
    xdgdesktopfile.h
    xdgdirs.h
    xdgicon.h
    xdgmenu.h
    xdgmenuwidget.h
    xdgmime.h
    xmlhelper.h
)

set(QTXDG_PRIVATE_HDRS
    xdgmenuapplinkprocessor.h
    xdgmenulayoutprocessor.h
    xdgmenu_p.h
    xdgmenureader.h
    xdgmenurules.h
)

set(QTXDG_SRCS
    xdgaction.cpp
    xdgdesktopfile.cpp
    xdgdirs.cpp
    xdgicon.cpp
    xdgmenuapplinkprocessor.cpp
    xdgmenu.cpp
    xdgmenulayoutprocessor.cpp
    xdgmenureader.cpp
    xdgmenurules.cpp
    xdgmenuwidget.cpp
    xdgmime.cpp
    xmlhelper.cpp
)

set(QTXDG_MOCS
    xdgaction.h
    xdgdesktopfile.h
    xdgmenuapplinkprocessor.h
    xdgmenu.h
    xdgmenu_p.h
    xdgmenureader.h
    xdgmenurules.h
    xdgmenuwidget.h
)

set(QT_USE_QTXML True)

# Translations **********************************
file(GLOB TS_FILES
    translations/*.ts
)
#************************************************


#**********************************************************
cmake_minimum_required( VERSION 2.6 )

QT4_WRAP_CPP(QTXDG_CXX ${QTXDG_MOCS})
qt4_add_translation(QM_FILES ${TS_FILES})

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)


# some system (rpm builds) setup LIB_SUFFIX for cmake. If there is no set, try to get it from system
IF (NOT DEFINED LIB_SUFFIX)
    MESSAGE(STATUS "*********************************************************************")
    MESSAGE(STATUS "LIB_SUFFIX variable is not defined. It will be autodetected now")
    MESSAGE(STATUS "You can set it manually with -DLIB_SUFFIX=<value> (64 for example)")

    # All 32bit system have empty lib suffix
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        # If there is lib64 dir, set suffix to 64
        if(IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib64)
            set(LIB_SUFFIX 64)
        elseif(IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)
            set(LIB_SUFFIX "")
        else()
            message(WARNING "LIB_SUFFIX cannot be autodetected. No ${CMAKE_INSTALL_PREFIX}/lib neither ${CMAKE_INSTALL_PREFIX}/lib64 found.")
            set(LIB_SUFFIX "")
        endif()
    else()
        set(LIB_SUFFIX "")
    endif()

    message(STATUS "LIB_SUFFIX autodetected as '${LIB_SUFFIX}', libraries will be installed into ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
    MESSAGE(STATUS "*********************************************************************")
ENDIF (NOT DEFINED LIB_SUFFIX)


set(APP_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/qtxdg)
add_definitions(-DTRANSLATIONS_DIR=\"${APP_SHARE_DIR}\")

add_library(qtxdg SHARED ${QTXDG_PUBLIC_HDRS} ${QTXDG_PRIVATE_HDRS} ${QTXDG_SRCS} ${QTXDG_CXX} ${QM_FILES})
target_link_libraries(qtxdg  ${QT_LIBRARIES} ${X11_X11_LIB} ${LIBMAGIC_LIBRARY})

set_target_properties(qtxdg PROPERTIES
  VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
  SOVERSION ${MAJOR_VERSION}
)

install(TARGETS qtxdg DESTINATION lib${LIB_SUFFIX})
install(FILES ${QTXDG_PUBLIC_HDRS} DESTINATION include/qtxdg)
install(FILES   ${QM_FILES}        DESTINATION ${APP_SHARE_DIR})

#find_package(Doxygen)
#if(DOXYGEN_FOUND)

#    configure_file(
#        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
#        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY
#    )
#    add_custom_target(doc
#        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        COMMENT "Generating API documentation with Doxygen" VERBATIM
#    )

#endif(DOXYGEN_FOUND)
