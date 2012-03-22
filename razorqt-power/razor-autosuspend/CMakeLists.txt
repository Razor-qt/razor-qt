set(PROJECT razor-autosuspend)
project($PROJECT)
include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/
        )

set ( razor-autosuspend_HDRS
        )

set ( razor-autosuspend_SRCS
        main.cpp
        trayicon.cpp
        settings_dialog.cpp
        org_freedesktop_UPower.cpp
        )

set ( razor-autosuspend_UIS
        settings_dialog.ui
        )
QT4_WRAP_UI(UIS ${razor-autosuspend_UIS})

set ( razor-autosuspend_RSCS
        razor-autosuspend.qrc
        )
QT4_ADD_RESOURCES(RSCS ${razor-autosuspend_RSCS})

set ( razor-autosuspend_MOCS
        trayicon.h
        settings_dialog.h
        org_freedesktop_UPower.h
        )
QT4_WRAP_CPP(MOCS ${razor-autosuspend_MOCS})

file(GLOB DESKTOP_FILES_IN
    resources/*.desktop.in
)

# Translations **********************************
include(RazorTranslate)
razor_translate(QM_FILES)
razor_translate_desktop(DESKTOP_FILES ${DESKTOP_FILES_IN})
#************************************************

add_executable ( razor-autosuspend ${razor-autosuspend_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} ${DESKTOP_FILES})
target_link_libraries ( razor-autosuspend  razorqt ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} )

file(GLOB ICON_FILES
    icons/*.svg)

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES   ${DESKTOP_FILES}   DESTINATION share/applications)
install(FILES   ${ICON_FILES}      DESTINATION share/icons/hicolor/scalable/apps)
