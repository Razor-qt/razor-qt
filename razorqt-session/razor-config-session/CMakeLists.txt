project(razor-config-session)

include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/libraries/)

set( razor-sessioncfg_HDRS
    sessionconfigwindow.h
    basicsettings.h
    autostartpage.h
    autostartmodel.h
    autostartedit.h
    defaultappspage.h
    environmentpage.h
)

set( razor-sessioncfg_SRCS
    main.cpp
    sessionconfigwindow.cpp
    basicsettings.cpp
    autostartpage.cpp
    autostartmodel.cpp
    autostartedit.cpp
    defaultappspage.cpp
    environmentpage.cpp
    ../razor-session/src/windowmanager.cpp
)
set(razor-sessioncfg_UI_FILES
    basicsettings.ui
    autostartpage.ui
    autostartedit.ui
    defaultappspage.ui
    environmentpage.ui
)

QT4_WRAP_UI(razor-sessioncfg_UIS ${razor-sessioncfg_UI_FILES})
QT4_WRAP_CPP(razor-sessioncfg_MOCS ${razor-sessioncfg_HDRS})
# Translations **********************************
include(RazorTranslate)
razor_translate_ts(razor-sessioncfg_QM_FILES 
    SOURCES
        ${razor-sessioncfg_HDRS}
        ${razor-sessioncfg_SRCS} 
        ${razor-sessioncfg_UI_FILES}
)

razor_translate_desktop2(razor-sessioncfg_DESKTOP_FILES
    SOURCES
        razor-config-session.desktop.in
) 
#************************************************

add_executable ( razor-config-session ${razor-sessioncfg_SRCS} ${razor-sessioncfg_UIS} ${razor-sessioncfg_MOCS} ${razor-sessioncfg_DESKTOP_FILES} ${razor-sessioncfg_QM_FILES})
add_dependencies( razor-config-session razorqt qtxdg)
target_link_libraries ( razor-config-session  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTXML_LIBRARY} ${X11_X11_LIB} razorqt qtxdg)

INSTALL(TARGETS razor-config-session RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
