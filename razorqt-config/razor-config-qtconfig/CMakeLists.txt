project(razor-config-qtconfig)

set(DESKTOP_FILES ${PROJECT_NAME}.desktop)

install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
razor_config_register(${DESKTOP_FILES})


