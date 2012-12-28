cmake_minimum_required( VERSION 2.6 )

# Translations **********************************
include(RazorTranslate)

file(GLOB DIR_IN_FILES "*.directory.in")
razor_translate_desktop(DIR_TR_FILES
    SOURCES
        ${DIR_IN_FILES}
)

add_custom_target(resources_directory_files ALL DEPENDS ${DIR_TR_FILES})

install(FILES ${DIR_TR_FILES} DESTINATION share/desktop-directories/)
