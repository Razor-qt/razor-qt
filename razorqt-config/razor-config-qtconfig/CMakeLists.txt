project(razor-config-qtconfig)

# Translations **********************************
include(RazorTranslate)
razor_translate_desktop(DESKTOP_FILES
    SOURCES
        ${PROJECT_NAME}.desktop.in
)

add_custom_target(${PROJECT_NAME} ALL DEPENDS ${DESKTOP_FILES})

install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
