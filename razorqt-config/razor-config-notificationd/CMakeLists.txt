project(razor-config-notificationd)

include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/
    ../../libraries/razorqt
    )

set(razor_config_notificationd_hdrs
   )

set(razor_config_notificationd_mocs
    mainwindow.h
)

set(razor_config_notificationd_srcs
    mainwindow.cpp
    main.cpp
   )

set(razor_config_notificationd_ui
    mainwindow.ui
)

set(razor_config_notificationd_qrc
)

# Translations **********************************
include(RazorTranslate)
razor_translate_desktop(DESKTOP_FILES razor-config-notificationd.desktop.in)
#************************************************

qt4_wrap_cpp(razor_config_notificationd_mocs_srcs ${razor_config_notificationd_mocs})
qt4_wrap_ui(razor_config_notificationd_ui_hdrs ${razor_config_notificationd_ui})
qt4_add_resources(razor_config_notificationd_qrc_srcs ${razor_config_notificationd_qrc})

set(razor_config_notificationd_srcs
    ${razor_config_notificationd_srcs}
    ${razor_config_notificationd_mocs_srcs}
    ${razor_config_notificationd_ui_hdrs}
    ${razor_config_notificationd_qrc_srcs}
    ${DESKTOP_FILES}
)
    

add_executable(${PROJECT_NAME} ${razor_config_notificationd_srcs} )
add_dependencies(${PROJECT_NAME} razorqt )
target_link_libraries(${PROJECT_NAME}  ${QT_LIBRARIES} razorqt)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
