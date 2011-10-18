MACRO(razor_translate_to _QM_FILES _TRANSLATIONS_DIR)
    file(GLOB TS_FILES
        translations/*.ts
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
