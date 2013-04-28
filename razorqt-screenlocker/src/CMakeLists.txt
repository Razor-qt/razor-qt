set(PROJECT razor-screenlocker)
project(${PROJECT})

set(MOC_FILES
    razorscreenlocker.h
)

set(H_FILES
    ${MOC_FILES}
    x11helper.h
)

set(CPP_FILES
    main.cpp
    razorscreenlocker.cpp
    x11helper.cpp
)

set(LIBRARIES
    razorqt
)

file(GLOB DESKTOP_FILES_IN
    resources/*.desktop.in
)

# Translations **********************************
include(RazorTranslate)
razor_translate_ts(QM_FILES
    SOURCES
        ${H_FILES}
        ${CPP_FILES}
        ${UI_FILES}
)

razor_translate_desktop(DESKTOP_FILES
    SOURCES
        ${DESKTOP_FILES_IN}
)
#************************************************

qt4_wrap_cpp(MOC_SOURCES ${MOC_FILES})
qt4_add_dbus_adaptor(CPP_FILES
    ../../razorqt-resources/dbus/org.freedesktop.ScreenSaver.xml
    razorscreenlocker.h RazorScreenLocker
)

# XCB stuff (TODO: move into findXCB)
find_package(PkgConfig)

pkg_check_modules(PKG_XCB xcb)
find_path(XCB_INCLUDE_DIR xcb/xcb.h
    ${PKG_XCB_INCLUDE_DIRS}
)
find_library(XCB_LIBRARY NAMES xcb
    PATHS
    ${PKG_XCB_LIBRARY_DIRS}
)

pkg_check_modules(PKG_X11_XCB x11-xcb)
find_path(X11_XCB_INCLUDE_DIR X11/Xlib-xcb.h
    ${PKG_X11_XCB_INCLUDE_DIRS}
)
find_library(X11_XCB_LIBRARY NAMES X11-xcb
    PATHS
    ${PKG_X11_XCB_LIBRARY_DIRS}
)

pkg_check_modules(PKG_XCB_SCREENSAVER xcb-screensaver)
find_path(XCB_SCREENSAVER_INCLUDE_DIR xcb/screensaver.h
    ${PKG_XCB_SCREENSAVER_INCLUDE_DIRS}
)
find_library(XCB_SCREENSAVER_LIBRARY NAMES xcb-screensaver
    PATHS
    ${PKG_XCB_SCREENSAVER_LIBRARY_DIRS}
)

pkg_check_modules(PKG_XCB_DPMS xcb-dpms)
find_path(XCB_DPMS_INCLUDE_DIR xcb/dpms.h
    ${PKG_XCB_SCREENSAVER_INCLUDE_DIRS}
)
find_library(XCB_DPMS_LIBRARY NAMES xcb-dpms
    PATHS
    ${PKG_XCB_DPMS_LIBRARY_DIRS}
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${XCB_INCLUDE_DIR}
    ${X11_XCB_INCLUDE_DIR}
    ${XCB_SCREENSAVER_INCLUDE_DIR}
    ${XCB_DPMS_INCLUDE_DIR}
    ../../libraries # TODO: Remove me
)

add_executable(${PROJECT} ${CPP_FILES} ${QM_FILES} ${MOC_SOURCES} ${DESKTOP_FILES})
add_dependencies(${PROJECT} razorqt)
target_link_libraries(${PROJECT}
                        ${LIBRARIES}
                        ${QT_LIBRARIES}
                        ${XCB_LIBRARY}
                        ${XCB_SCREENSAVER_LIBRARY}
                        ${XCB_DPMS_LIBRARY}
                        ${X11_XCB_LIBRARY}
)

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES   ${CONFIG_FILES}    DESTINATION ${APP_SHARE_DIR})
install(FILES   ${DESKTOP_FILES}   DESTINATION share/applications)
