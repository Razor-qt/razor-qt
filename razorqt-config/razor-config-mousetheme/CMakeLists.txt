
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/librazorqt/
    .
    ${CMAKE_CURRENT_SOURCE_DIR}/xcr
    )

set ( razor-config-mousetheme_HDRS
    cfgfile.h
    thememodel.h
    previewwidget.h
    xcr/xcrtheme.h
    xcr/xcrimg.h
    xcr/xcrthemexp.h
    xcr/xcrthemefx.h
    xcr/xcrxcur.h
    itemdelegate.h
    main.h
    crtheme.h
    selectwnd.h
	)

set ( razor-config-mousetheme_SRCS
    crtheme.cpp
    selectwnd.cpp
    xcr/xcrimg.cpp
    xcr/xcrxcur.cpp
    xcr/xcrthemefx.cpp
    xcr/xcrtheme.cpp
    xcr/xcrthemexp.cpp
    main.cpp
    cfgfile.cpp
    previewwidget.cpp
    itemdelegate.cpp
    thememodel.cpp
	)

QT4_WRAP_UI(razor-config-mousetheme_UIS     ui/selectwnd.ui)
QT4_WRAP_CPP(razor-config-mousetheme_MOCS ${razor-config-mousetheme_HDRS})

add_executable ( razor-config-mousetheme ${razor-config-mousetheme_SRCS} ${razor-config-mousetheme_UIS} ${razor-config-mousetheme_MOCS} )
add_dependencies( razor-config-mousetheme razorqt qtxdg)
target_link_libraries ( razor-config-mousetheme  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTXML_LIBRARY} ${X11_X11_LIB})

INSTALL(TARGETS razor-config-mousetheme RUNTIME DESTINATION bin)
install(FILES razor-config-mousetheme.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)

razor_config_register(${CMAKE_INSTALL_PREFIX}/share/applications/razor-config-mousetheme.desktop)
