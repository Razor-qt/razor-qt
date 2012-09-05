cmake_minimum_required( VERSION 2.6 )

file(GLOB CONF_FILES *.conf)
file(GLOB CONF_FILES_IN *.conf.in)

# Translations **********************************
include(RazorTranslate)
razor_translate_desktop(CONF_TR_FILES
    SOURCES
        ${CONF_FILES_IN}
)
add_custom_target(resources_conf_files ALL DEPENDS ${CONF_TR_FILES})
#************************************************

install(FILES ${CONF_FILES}    DESTINATION ${RAZOR_ETC_XDG_DIRECTORY}/razor)
install(FILES ${CONF_TR_FILES} DESTINATION ${RAZOR_ETC_XDG_DIRECTORY}/razor)
