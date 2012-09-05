include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/libraries/)

set ( razor-desktopcfg_HDRS
    desktopconfigwindow.h
    basicsettings.h
    menuconfig.h
    wmnativeconfig.h
    )

set ( razor-desktopcfg_SRCS
    main.cpp
    desktopconfigwindow.cpp
    basicsettings.cpp
    menuconfig.cpp
    wmnativeconfig.cpp
    )

QT4_WRAP_UI(razor-desktopcfg_UIS
    basicsettings.ui
    menuconfig.ui
    wmnativeconfig.ui
    )

QT4_WRAP_CPP(razor-desktopcfg_MOCS ${razor-desktopcfg_HDRS})

set(DESKTOP_FILES_IN
    razor-config-desktop.desktop.in
)

# Translations **********************************
include(RazorTranslate)
razor_translate_desktop(DESKTOP_FILES
    SOURCES
        ${DESKTOP_FILES_IN}
)
#************************************************

add_executable ( razor-config-desktop ${razor-desktopcfg_SRCS} ${razor-desktopcfg_UIS} ${razor-desktopcfg_MOCS} ${DESKTOP_FILES} )
add_dependencies( razor-config-desktop razorqt qtxdg)
target_link_libraries ( razor-config-desktop  ${QT_QTCORE_LIBRARY}
                            ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY}
                            ${QT_QTXML_LIBRARY} ${X11_X11_LIB} razorqt qtxdg)

INSTALL(TARGETS razor-config-desktop RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
