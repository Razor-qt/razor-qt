include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/librazorqt/ .)

set ( razor-sessioncfg_HDRS
    sessionconfigwindow.h
	)

set ( razor-sessioncfg_SRCS
	main.cpp
    sessionconfigwindow.cpp
	)

QT4_WRAP_UI(razor-sessioncfg_UIS sessionconfigwindow.ui)
QT4_WRAP_CPP(razor-sessioncfg_MOCS ${razor-sessioncfg_HDRS})

add_executable ( razor-config-session ${razor-sessioncfg_SRCS} ${razor-sessioncfg_UIS} ${razor-sessioncfg_MOCS} )
add_dependencies( razor-config-session razorqt qtxdg)
target_link_libraries ( razor-config-session  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTXML_LIBRARY} ${X11_X11_LIB} razorqt qtxdg)

INSTALL(TARGETS razor-config-session RUNTIME DESTINATION bin)
install(FILES razor-config-session.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
