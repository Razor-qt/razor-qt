include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/libraries/)

set ( razor-desktopcfg_HDRS
    desktopconfigwindow.h
	)

set ( razor-desktopcfg_SRCS
	main.cpp
    desktopconfigwindow.cpp
	)

QT4_WRAP_UI(razor-desktopcfg_UIS desktopconfigwindow.ui)
QT4_WRAP_CPP(razor-desktopcfg_MOCS ${razor-desktopcfg_HDRS})

# Translations **********************************
include(RazorTranslate)
razor_translate_desktop(DESKTOP_FILES razor-config-desktop.desktop.in)
#************************************************

add_executable ( razor-config-desktop ${razor-desktopcfg_SRCS} ${razor-desktopcfg_UIS} ${razor-desktopcfg_MOCS} ${DESKTOP_FILES} )
add_dependencies( razor-config-desktop razorqt qtxdg)
target_link_libraries ( razor-config-desktop  ${QT_QTCORE_LIBRARY}
                            ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY}
                            ${QT_QTXML_LIBRARY} ${X11_X11_LIB} razorqt qtxdg)

INSTALL(TARGETS razor-config-desktop RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)

#razor_config_register(${CMAKE_INSTALL_PREFIX}/share/applications/razor-config-desktop.desktop)
razor_config_register(${DESKTOP_FILES})

