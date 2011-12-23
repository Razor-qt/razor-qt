project(libqtxdg)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set (MAJOR_VERSION 0)
set (MINOR_VERSION 1)
set (PATCH_VERSION 0)


add_definitions(-Wall)
find_package(Qt4 REQUIRED)
find_package(LibMagic REQUIRED)


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
    qiconfix/qiconloader_p.h
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
    qiconfix/qiconloader.cpp
)

set(QTXDG_MOCS
    xdgaction.h
    xdgmenuapplinkprocessor.h
    xdgmenu.h
    xdgmenu_p.h
    xdgmenureader.h
    xdgmenurules.h
    xdgmenuwidget.h
)

set(QT_USE_QTXML TRUE)


# Translations **********************************
file(GLOB QTXDG_TS_FILES
    translations/*.ts
)
#************************************************


#**********************************************************
cmake_minimum_required( VERSION 2.6 )

include(RazorLibSuffix)
include(${QT_USE_FILE})

QT4_WRAP_CPP(QTXDG_CXX ${QTXDG_MOCS})
qt4_add_translation(QTXDG_QM_FILES ${QTXDG_TS_FILES})

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)


set(APP_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/qtxdg)
add_definitions(-DTRANSLATIONS_DIR=\"${APP_SHARE_DIR}\")

add_library(qtxdg SHARED ${QTXDG_PUBLIC_HDRS} ${QTXDG_PRIVATE_HDRS} ${QTXDG_SRCS} ${QTXDG_CXX} ${QTXDG_QM_FILES})
target_link_libraries(qtxdg  ${QT_LIBRARIES} ${LIBMAGIC_LIBRARY})

set_target_properties(qtxdg PROPERTIES
  VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
  SOVERSION ${MAJOR_VERSION}
)

install(TARGETS qtxdg DESTINATION lib${LIB_SUFFIX})
install(FILES ${QTXDG_PUBLIC_HDRS} DESTINATION include/qtxdg)
install(FILES ${QTXDG_QM_FILES}    DESTINATION ${APP_SHARE_DIR})

include(create_pkgconfig_file)
create_pkgconfig_file(qtxdg "QtXdg, a Qt implementation of XDG standards")
