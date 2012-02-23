#**********************************************************
# TS files
#**********************************************************

MACRO(razor_translate_to _QM_FILES _TRANSLATIONS_DIR)
    file(GLOB TS_FILES
        translations/*.ts
        translations/local/*.ts
    )

    set(TRANSLATIONS_DIR ${_TRANSLATIONS_DIR})
    add_definitions(-DTRANSLATIONS_DIR=\"${TRANSLATIONS_DIR}\")
    qt4_add_translation(${_QM_FILES} ${TS_FILES})


    install(FILES ${${_QM_FILES}} DESTINATION ${TRANSLATIONS_DIR})

    configure_file(${CMAKE_MODULE_PATH}/razortranslate.h.in razortranslate.h)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
ENDMACRO(razor_translate_to)


MACRO(razor_translate _QM_FILES)
    razor_translate_to(${_QM_FILES} ${CMAKE_INSTALL_PREFIX}/share/razor/${PROJECT_NAME})
ENDMACRO(razor_translate)


#**********************************************************
# DESCTOP files
#**********************************************************

MACRO(razor_translate_desktop _RESULT)
  foreach (_current_FILE ${ARGN})
    get_filename_component(_IN_FILE ${_current_FILE} ABSOLUTE)
    get_filename_component(_FILE_NAME ${_IN_FILE} NAME_WE)
    #Extract real extension ................
    get_filename_component(_FILE_EXT ${_IN_FILE} EXT)
    string(REPLACE ".in" "" _FILE_EXT ${_FILE_EXT})
    #.......................................
    set(_OUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${_FILE_NAME}${_FILE_EXT}")

    
    file(GLOB TR_FILES
        translations/${_FILE_NAME}_*${_FILE_EXT}
        translations/local/${_FILE_NAME}_*${_FILE_EXT}
    )

    if (NOT TR_FILES)
        message(FATAL_ERROR "Translations files not found for ${_current_FILE}")
    endif()

    #message(status ".:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:")
    #message(status "EXT ${_FILE_EXT}")
    #message(status "IN:     ${_IN_FILE}")
    #message(status "OUT:    ${_OUT_FILE}")
    #message(status "TR Dir: ${CMAKE_CURRENT_SOURCE_DIR}")
    #message(status "TR:  ${TR_FILES}")
    #message(status ".:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:")

    set(PATTERN "'\\[.*]\\s*='")
    add_custom_command(OUTPUT ${_OUT_FILE}
        COMMAND grep -v "'#TRANSLATIONS_DIR='" ${_IN_FILE} > ${_OUT_FILE}
        COMMAND grep --no-filename -P ${PATTERN} ${TR_FILES} >> ${_OUT_FILE}
        COMMENT "Generating ${_FILE_NAME}${_FILE_EXT}"
    )

    SET(${_RESULT} ${${_RESULT}} ${_OUT_FILE})
  endforeach(_current_FILE)
ENDMACRO(razor_translate_desktop)
