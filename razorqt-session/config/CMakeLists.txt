include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/libraries/)

set ( razor-sessioncfg_HDRS
    sessionconfigwindow.h
    autostartmodel.h
    autostartedit.h
	)

set ( razor-sessioncfg_SRCS
    main.cpp
    sessionconfigwindow.cpp
    autostartmodel.cpp
    autostartedit.cpp
    ../src/windowmanager.cpp
	)

QT4_WRAP_UI(razor-sessioncfg_UIS
    sessionconfigwindow.ui
    autostartedit.ui
        )
QT4_WRAP_CPP(razor-sessioncfg_MOCS ${razor-sessioncfg_HDRS})
# Translations **********************************
include(RazorTranslate)
razor_translate_desktop(DESKTOP_FILES razor-config-session.desktop.in)
#************************************************

add_executable ( razor-config-session ${razor-sessioncfg_SRCS} ${razor-sessioncfg_UIS} ${razor-sessioncfg_MOCS} ${DESKTOP_FILES})
add_dependencies( razor-config-session razorqt qtxdg)
target_link_libraries ( razor-config-session  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTXML_LIBRARY} ${X11_X11_LIB} razorqt qtxdg)

INSTALL(TARGETS razor-config-session RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
