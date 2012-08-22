project( razor-session )
cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
   set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( X11 REQUIRED )

include(RazorLibSuffix)

include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/libraries/
    src)

set ( razor-session_HDRS
	)

set ( razor-session_SRCS
    src/main.cpp
    src/razormodman.cpp
    src/wmselectdialog.cpp
    src/windowmanager.cpp
	)

set ( razor-session_MOCS
    src/razormodman.h
    src/sessiondbusadaptor.h
    src/wmselectdialog.h
	)

set ( razor-session_UI
    src/wmselectdialog.ui
)

# Translations **********************************
include(RazorTranslate)
razor_translate_ts(razor-runner_QM_FILES 
    SOURCES
        ${razor-session_HDRS}
        ${razor-session_SRCS} 
        ${razor-session_UI}
)

#************************************************

QT4_WRAP_CPP(MOCS ${razor-session_MOCS})
qt4_wrap_ui(UIS ${razor-session_UI})

add_executable ( razor-session ${razor-session_SRCS} ${UIS} ${RSCS} ${razor-runner_QM_FILES} ${MOCS} )
add_dependencies( razor-session razorqt qtxdg)
target_link_libraries ( razor-session  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTXML_LIBRARY} ${X11_X11_LIB} razorqt qtxdg)
INSTALL(TARGETS razor-session RUNTIME DESTINATION bin)

if (NOT DEFINED BUNDLE_XDG_UTILS)
    message(STATUS "*********************************************************************")
    message(STATUS "Bundling our own xdg-utils.")
    message(STATUS "If you have xdg-utils newer than 2012-03-02, you can set -DBUNDLE_XDG_UTILS=No")
    message(STATUS "*********************************************************************")
    set (BUNDLE_XDG_UTILS Yes)
endif (NOT DEFINED BUNDLE_XDG_UTILS)

if (BUNDLE_XDG_UTILS)
    install(PROGRAMS resources/xdg-open resources/xdg-mime DESTINATION lib${LIB_SUFFIX}/razor-xdg-tools)
    add_definitions ( -DPATH_PREPEND=\"${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-xdg-tools\" )
endif (BUNDLE_XDG_UTILS)

install(PROGRAMS src/startrazor DESTINATION bin)

