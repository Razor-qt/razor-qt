project(razor-config-notificationd)

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/
    )

set(NOTIFICATIONS_CONF_MOC
    advancedsettings.h
    basicsettings.h
    mainwindow.h
)

set(NOTIFICATIONS_CONF_SRC
    advancedsettings.cpp
    basicsettings.cpp
    mainwindow.cpp
    main.cpp
   )

set(NOTIFICATIONS_CONF_UI
    advancedsettings.ui
    basicsettings.ui
)


# Translations **********************************
include(RazorTranslate)
razor_translate_desktop(DESKTOP_FILES
    SOURCES
        razor-config-notificationd.desktop.in
)
#************************************************

qt4_wrap_cpp(NOTIFICATIONS_CONF_MOC_CPP ${NOTIFICATIONS_CONF_MOC})
qt4_wrap_ui(NOTIFICATIONS_CONF_UI_CPP ${NOTIFICATIONS_CONF_UI})

add_executable(${PROJECT_NAME} ${NOTIFICATIONS_CONF_SRC}
                               ${NOTIFICATIONS_CONF_MOC_CPP}
                               ${NOTIFICATIONS_CONF_UI_CPP}
                               ${DESKTOP_FILES})
add_dependencies(${PROJECT_NAME} razorqt )
target_link_libraries(${PROJECT_NAME} ${QT_QTCORE_LIBRARY}
                                      ${QT_QTGUI_LIBRARY}
                                      ${QT_QTDBUS_LIBRARY}
                                      razorqt)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
