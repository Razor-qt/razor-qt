project(libqtxdg)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set (MAJOR_VERSION 0)
set (MINOR_VERSION 5)
set (PATCH_VERSION 2)


add_definitions(-Wall)
find_package(Qt4 REQUIRED)
find_package(LibMagic REQUIRED)


set(libqtxdg_PUBLIC_H_FILES
    xdgaction.h
    xdgdesktopfile.h
    xdgdirs.h
    xdgicon.h
    xdgmenu.h
    xdgmenuwidget.h
    xdgmime.h
    xmlhelper.h
    xdgautostart.h
)

set(libqtxdg_PRIVATE_H_FILES
    xdgmenuapplinkprocessor.h
    xdgmenulayoutprocessor.h
    xdgmenu_p.h
    xdgmenureader.h
    xdgmenurules.h
    qiconfix/qiconloader_p.h
)

set(libqtxdg_CPP_FILES
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
    xdgautostart.cpp
    qiconfix/qiconloader.cpp
)

set(libqtxdg_MOCS
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
# QtXdg is an independent library, so I do not use razor_translate_ts
include(qtxdgTranslate)
qtxdg_translate_ts(libqtxdg_QM_FILES 
    SOURCES
        ${libqtxdg_PUBLIC_H_FILES}
        ${libqtxdg_PRIVATE_H_FILES}
        ${libqtxdg_CPP_FILES} 
        ${libqtxdg_UI_FILES}
)

#************************************************


#**********************************************************
cmake_minimum_required( VERSION 2.6 )

include(RazorLibSuffix)
include(${QT_USE_FILE})

QT4_WRAP_CPP(libqtxdg_CXX_FILES ${libqtxdg_MOCS})

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)


set(APP_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/libqtxdg)
add_definitions(-DTRANSLATIONS_DIR=\"${APP_SHARE_DIR}\")

add_library(qtxdg SHARED 
    ${libqtxdg_PUBLIC_H_FILES} 
    ${libqtxdg_PRIVATE_H_FILES} 
    ${libqtxdg_CPP_FILES} 
    ${libqtxdg_CXX_FILES} 
    ${libqtxdg_QM_FILES}
)
target_link_libraries(qtxdg  ${QT_LIBRARIES} ${LIBMAGIC_LIBRARY})

set_target_properties(qtxdg PROPERTIES
  VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
  SOVERSION ${MAJOR_VERSION}
)

install(TARGETS qtxdg DESTINATION lib${LIB_SUFFIX})
install(FILES ${libqtxdg_PUBLIC_H_FILES} DESTINATION include/qtxdg)

include(create_pkgconfig_file)
create_pkgconfig_file(qtxdg "QtXdg, a Qt implementation of XDG standards")
