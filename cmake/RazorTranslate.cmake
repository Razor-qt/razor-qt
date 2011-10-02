MACRO(razor_translate _QM_FILES)
    #MESSAGE(STATUS "* Translate *********************************************************")
    #MESSAGE(STATUS "TRANSLATE ${PROJECT_NAME}: ${CMAKE_CURRENT_LIST_DIR}")

    file(GLOB TS_FILES
        translations/*.ts
    )

    set(TRANSLATIONS_DIR
        ${CMAKE_INSTALL_PREFIX}/share/razor/${PROJECT_NAME}
    )
    
    add_definitions(-DTRANSLATIONS_DIR=\"${TRANSLATIONS_DIR}\")
    qt4_add_translation(${_QM_FILES} ${TS_FILES})

    install(FILES ${${_QM_FILES}} DESTINATION ${TRANSLATIONS_DIR})    
    
    configure_file(${CMAKE_MODULE_PATH}/razortranslate.h.in razortranslate.h)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    #MESSAGE(STATUS "*********************************************************************")
ENDMACRO(razor_translate) 

