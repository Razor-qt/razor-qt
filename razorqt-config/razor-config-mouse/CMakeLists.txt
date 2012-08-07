find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/libraries/
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/xcr
    )

set ( razor-config-mouse_HDRS
#    cfgfile.h
    thememodel.h
    previewwidget.h
#    xcr/xcrtheme.h
#    xcr/xcrimg.h
#    xcr/xcrthemexp.h
#    xcr/xcrthemefx.h
#    xcr/xcrxcur.h
    itemdelegate.h
#    main.h
#    crtheme.h
    selectwnd.h
	)

set ( razor-config-mouse_SRCS
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

find_package(ZLIB REQUIRED)
QT4_WRAP_UI(razor-config-mouse_UIS     ui/selectwnd.ui)
QT4_WRAP_CPP(razor-config-mouse_MOCS ${razor-config-mouse_HDRS})
# Translations **********************************
include(RazorTranslate)
razor_translate(QM_FILES)
razor_translate_desktop(DESKTOP_FILES razor-config-mouse.desktop.in)
#************************************************

add_executable ( razor-config-mouse ${razor-config-mouse_SRCS} ${razor-config-mouse_UIS} ${razor-config-mouse_MOCS} ${DESKTOP_FILES})
add_dependencies( razor-config-mouse razorqt qtxdg)
target_link_libraries ( razor-config-mouse  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTXML_LIBRARY}
                                                 ${X11_X11_LIB} ${X11_Xcursor_LIB} razorqt ${ZLIB_LIBRARY} ${X11_Xfixes_LIB})
# not needed probably ${X11_Xfixes_LIB})

INSTALL(TARGETS razor-config-mouse RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
