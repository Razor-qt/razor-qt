# build static helper class first
project(razor-config)
add_subdirectory(qcategorizedview)

include_directories (
        ${CMAKE_CURRENT_BINARY_DIR}
        ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/libraries
        ${CMAKE_CURRENT_SOURCE_DIR}/qcategorizedview
        )

set ( razor-config_HDRS
	)

set ( razor-config_SRCS
	main.cpp
	mainwindow.cpp
	)

set ( razor-config_MOCS
    mainwindow.h
	)

set( razor-config_UIS
    mainwindow.ui
)
QT4_WRAP_CPP(MOCS ${razor-config_MOCS})
QT4_WRAP_UI(UIS ${razor-config_UIS})
# Translations **********************************
include(RazorTranslate)
razor_translate_ts(QM_FILES
    SOURCES
        ${razor-config_HDRS}
        ${razor-config_SRCS}
        ${razor-config_UIS}
)

razor_translate_desktop(DESKTOP_FILES
    SOURCES
        razor-config.desktop.in
)
#************************************************

add_executable ( razor-config ${razor-config_SRCS} ${UIS} ${MOCS} ${DESKTOP_FILES} ${QM_FILES})
add_dependencies( razor-config razorqt qtxdg)
target_link_libraries ( razor-config
                ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY}
                qtxdg razorqt)
# helper static lib
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/qcategorizedview)
target_link_libraries( razor-config qcategorizedview)

INSTALL(TARGETS razor-config RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(FILES razor-config.menu DESTINATION /etc/xdg/menus/)
