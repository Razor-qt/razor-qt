
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/librazorqt/
    .
    )

set ( razor-config-icons_HDRS
    mainwindow.h
	)

set ( razor-config-icons_SRCS
    mainwindow.cpp
    main.cpp
	)


QT4_WRAP_UI(razor-config-icons_UIS     mainwindow.ui)
QT4_WRAP_CPP(razor-config-icons_MOCS ${razor-config-icons_HDRS})

add_executable ( razor-config-icons ${razor-config-icons_SRCS} ${razor-config-icons_UIS} ${razor-config-icons_MOCS} )
add_dependencies( razor-config-icons razorqt qtxdg)
target_link_libraries ( razor-config-icons  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                                            razorqt qtxdg)
# not needed probably ${X11_Xfixes_LIB})

INSTALL(TARGETS razor-config-icons RUNTIME DESTINATION bin)
install(FILES razor-config-icons.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)

razor_config_register(razor-config-icons.desktop)


