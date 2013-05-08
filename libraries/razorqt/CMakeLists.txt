project(librazorqt)

set(razorqt_PUB_HDRS
    aboutdialog/razoraboutdlg.h
    razorsettings.h
    xfitman.h
    razorplugininfo.h
    powermanager.h
    addplugindialog/addplugindialog.h
    screensaver.h
    razorapplication.h
    # upower. Maybe standalone lib?
    #librazor-upower/battery.h
    #librazor-upower/ucpufreq.h
    #librazor-upower/upower.h
    libtranslate.h
    pageselectwidget.h
    programfinder.h
    razorconfigdialog.h
    razorpower/razorpower.h
    razornotification.h
    razorautostartentry.h
    translatorsinfo/translatorsinfo.h
    razorgridlayout.h
    rotatedwidget.h
)

set(razorqt_PUB_LINKS
    aboutdialog/RazorAboutDialog
)

set(razorqt_PRIV_HDRS
    aboutdialog/razoraboutdlg_p.h
    aboutdialog/technicalinfo.h
)

set(razorqt_SRCS
    aboutdialog/razoraboutdlg.cpp
    aboutdialog/technicalinfo.cpp
    xfitman.cpp
    razorplugininfo.cpp
    powermanager.cpp
    addplugindialog/addplugindialog.cpp
    razorsettings.cpp
    screensaver.cpp
    razorapplication.cpp
    # upower. Maybe standalone lib?
    #librazor-upower/battery.cpp
    #librazor-upower/ucpufreq.cpp
    #librazor-upower/upower.cpp
    pageselectwidget.cpp
    programfinder.cpp
    razorconfigdialog.cpp
    razorpower/razorpower.cpp
    razorpower/razorpowerproviders.cpp
    razornotification.cpp
    razorautostartentry.cpp
    translatorsinfo/translatorsinfo.cpp
    razorgridlayout.cpp
    rotatedwidget.cpp
)

set(razorqt_MOCS
    aboutdialog/razoraboutdlg.h
    aboutdialog/razoraboutdlg_p.h
    powermanager.h
    addplugindialog/addplugindialog.h
    razorsettings.h
    screensaver.h
    razorapplication.h
    # upower. Maybe standalone lib?
    #librazor-upower/battery.h
    #librazor-upower/ucpufreq.h
    #librazor-upower/upower.h
    pageselectwidget.h
    razorconfigdialog.h
    razornotification.h
    razornotification_p.h
    razorpower/razorpower.h
    razorpower/razorpowerproviders.h
    razorgridlayout.h
    rotatedwidget.h
)

QT4_ADD_DBUS_INTERFACE(razorqt_SRCS
    dbus/org.freedesktop.Notifications.xml
    notifications_interface
)

set( razorqt_UIS
    aboutdialog/razoraboutdlg.ui
    razorconfigdialog.ui
    addplugindialog/addplugindialog.ui
)


#message(STATUS "+++++++++++++++++++"  ${dbus_generated})
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(X11 REQUIRED)

find_package(QTXDG REQUIRED)
include(${QTXDG_USE_FILE})


include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR}
    ${X11_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/libraries
)

#************************************************
# Build 2 config.cmake files
# One for in-tree build and second for normal one.
#************************************************
set(RAZOR_MAJOR_VERSION ${MAJOR_VERSION})
set(RAZOR_MINOR_VERSION ${MINOR_VERSION})
set(RAZOR_PATCH_VERSION ${PATCH_VERSION})

set(RAZOR_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/razor/)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/RazorInstallConfigPath.cmake)

# In tree compilation ......................
set(RAZOR_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/aboutdialog
    ${CMAKE_CURRENT_SOURCE_DIR}/razorpower
    ${CMAKE_CURRENT_SOURCE_DIR}/translatorsinfo
    ${CMAKE_CURRENT_SOURCE_DIR}/addplugindialog
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/razor-config.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/razor-config.cmake
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/razor_use.cmake
    ${CMAKE_BINARY_DIR}/cmake/razor_use.cmake
    @ONLY
)


# Instalable ...............................
set(RAZOR_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/razorqt)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/razor-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/razor-config.cmake
    @ONLY
)
#************************************************
# End of build config.cmake
#************************************************

include_directories(${RAZOR_INCLUDE_DIR})

set(APP_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/librazorqt)
add_definitions(-DTRANSLATIONS_DIR=\"${APP_SHARE_DIR}\")

# Translations **********************************
include(cmake/razor_use.cmake)
razor_translate_ts(RAZORQT_QM_FILES
    SOURCES
        ${razorqt_PUB_HDRS}
        ${razorqt_PRIV_HDRS}
        ${razorqt_SRCS}
        ${razorqt_MOCS}
        ${razorqt_UIS}
    INSTALLATION_DIR
        ${APP_SHARE_DIR}
)

#************************************************


QT4_WRAP_CPP(MOCS ${razorqt_MOCS})
QT4_WRAP_UI(UIS ${razorqt_UIS})

include(translatorsinfo/CMakeLists.txt)
get_translatorsinfo_qrc(translatorsinfo_qrc)
qt4_add_resources(QRC_CXX_SOURCES ${translatorsinfo_qrc})

add_library(razorqt
    SHARED ${razorqt_PUB_HDRS}
           ${razorqt_PRIV_HDRS}
           ${razorqt_SRCS}
           ${dbus_generated}
           ${UIS} ${MOCS}
           ${RAZORQT_QM_FILES}
           ${QRC_CXX_SOURCES})

target_link_libraries(razorqt
    ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
    ${X11_X11_LIB}
    qtxdg
)

set_target_properties(razorqt PROPERTIES
    VERSION ${RAZOR_MAJOR_VERSION}.${RAZOR_MINOR_VERSION}.${RAZOR_PATCH_VERSION}
    SOVERSION ${RAZOR_MAJOR_VERSION}
)

install(TARGETS razorqt DESTINATION lib${LIB_SUFFIX})
install(FILES ${razorqt_PUB_HDRS}  DESTINATION include/razorqt)
install(FILES ${razorqt_PUB_LINKS} DESTINATION include/razorqt)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/razor-config.cmake DESTINATION share/cmake/razor-qt)
install(FILES cmake/razor_use.cmake DESTINATION share/cmake/razor-qt)

include(create_pkgconfig_file)
create_pkgconfig_file(razorqt "Shared library for Razor-qt applications")
