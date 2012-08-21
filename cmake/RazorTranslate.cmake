#  funtion razor_translate_ts(qm_files 
#                           SOURCES sources ... 
#                           [TRANSLATION_DIR] translation_directory
#                           [INSTALLATION_DIR] qm_install_directory
#                          )
#     out: qm_files 
#     generates commands to create .ts.src and .qm files from sources. 
#     The generated filenames can be found in qm_files.
#
#     in: sources 
#     List of the h, cpp and ui files
#
#     in: translation_directory
#     A relative path to the directory with .ts files, it is relative 
#     to the CMakeList.txt. By default is "translations"
#
#     in: qm_install_directory
#     A full path to the directory n which will be installed .qm files.
#     By default is "${CMAKE_INSTALL_PREFIX}/share/razor/${PROJECT_NAME}" 


MACRO(QT4_ADD_TRANSLATION_FIXED _qm_files)
  FOREACH (_current_FILE ${ARGN})
    GET_FILENAME_COMPONENT(_abs_FILE ${_current_FILE} ABSOLUTE)
    GET_FILENAME_COMPONENT(qm ${_abs_FILE} NAME)
    #Extract the real extension ............
    STRING(REPLACE ".ts" "" qm ${qm})
    GET_SOURCE_FILE_PROPERTY(output_location ${_abs_FILE} OUTPUT_LOCATION)
    IF(output_location)
      FILE(MAKE_DIRECTORY "${output_location}")
      SET(qm "${output_location}/${qm}.qm")
    ELSE(output_location)
      SET(qm "${CMAKE_CURRENT_BINARY_DIR}/${qm}.qm")
    ENDIF(output_location)

    ADD_CUSTOM_COMMAND(OUTPUT ${qm}
       COMMAND ${QT_LRELEASE_EXECUTABLE}
       ARGS ${_abs_FILE} -qm ${qm}
       DEPENDS ${_abs_FILE}
    )
    SET(${_qm_files} ${${_qm_files}} ${qm})
  ENDFOREACH (_current_FILE)
ENDMACRO(QT4_ADD_TRANSLATION_FIXED)

if(NOT TARGET UpdateTsFiles)
  add_custom_target(UpdateTsFiles DEPENDS)
endif()

if(NOT TARGET UpdateTxFile)
  add_custom_target(UpdateTxFile  
    COMMAND echo "[main]"                            > ${CMAKE_SOURCE_DIR}/.tx/config2
    COMMAND echo "host = https://www.transifex.net" >> ${CMAKE_SOURCE_DIR}/.tx/config2
    COMMAND echo ""                                 >> ${CMAKE_SOURCE_DIR}/.tx/config2
    COMMAND cat ${CMAKE_BINARY_DIR}/tx/*.tx_config  >> ${CMAKE_SOURCE_DIR}/.tx/config2
  )
endif()


function(razor_translate_ts _qmFiles)
    set(_translationDir "translations")
    set(_installDir "${CMAKE_INSTALL_PREFIX}/share/razor/${PROJECT_NAME}")
    
    # Parse arguments ***************************************
    set(_state "")
    foreach (_arg ${ARGN})
        if (
            ("${_arg}_I_HATE_CMAKE" STREQUAL "SOURCES_I_HATE_CMAKE") OR
            ("${_arg}_I_HATE_CMAKE" STREQUAL "TRANSLATION_DIR_I_HATE_CMAKE") OR
            ("${_arg}_I_HATE_CMAKE" STREQUAL "INSTALLATION_DIR_I_HATE_CMAKE") OR
            ("${_arg}_I_HATE_CMAKE" STREQUAL "TS_SRC_FILE_I_HATE_CMAKE")        
           )        
            set(_state ${_arg})
      
        else()
            if("${_state}" STREQUAL "SOURCES")
                get_filename_component (__file ${_arg} ABSOLUTE)
                set(_sources  ${_sources} ${__file})
                set(_sourcesSpace  "${_sourcesSpace} ${__file}")
 
            elseif("${_state}" STREQUAL "TRANSLATION_DIR")
                set(_translationDir ${_arg})       
                set(_state "")

            elseif("${_state}" STREQUAL "INSTALLATION_DIR")
                set(_installDir ${_arg})       
                set(_state "")

            elseif("${_state}" STREQUAL "TS_SRC_FILE")
                set(_tsSrcFile ${_arg})       
                set(_state "")
        
            else()  
                MESSAGE(FATAL_ERROR 
                  "Unknown argument '${_arg}'.\n"
                  "See ${CMAKE_CURRENT_LIST_FILE} for more information.\n"
                )
            endif()  
        endif()
    endforeach(_arg)

    get_filename_component (_translationDir ${_translationDir} ABSOLUTE)
    if ("${_tsSrcFile}" STREQUAL "") 
        set(_tsSrcFile  "${_translationDir}/${PROJECT_NAME}.ts.src")
    endif()
    
    get_filename_component (_tsSrcFile  ${_tsSrcFile} ABSOLUTE)
    get_filename_component (_tsSrcFileName  ${_tsSrcFile} NAME)
    get_filename_component (_tsSrcFileNameWE  ${_tsSrcFile} NAME_WE)
      
    # TS.SRC file *******************************************    
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/updateTsFile.sh
        "#/bin/sh\n"
        "\n"
        "mkdir -p ${_translationDir} 2>/dev/null\n"
        "cd ${_translationDir} && "
        "${QT_LUPDATE_EXECUTABLE} -target-language en_US ${_sourcesSpace} -ts ${_tsSrcFile}.ts &&"
        "mv ${_tsSrcFile}.ts ${_tsSrcFile}\n"
        "grep -q 'source' '${_tsSrcFile}' || rm '${_tsSrcFile}'\n"
    )

    add_custom_target(Update_${_tsSrcFileName}
        COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/updateTsFile.sh
        DEPENDS ${_sources}
        VERBATIM
    )
  
    add_dependencies(UpdateTsFiles Update_${_tsSrcFileName})
    
    # TX file ***********************************************
    set(_txFile "${CMAKE_BINARY_DIR}/tx/${_tsSrcFileName}.tx_config")  
    string(REPLACE "${CMAKE_SOURCE_DIR}/" "" _tx_translationDir ${_translationDir})
    string(REPLACE "${CMAKE_SOURCE_DIR}/" "" _tx_tsSrcFile ${_tsSrcFile})
    
    file(WRITE ${_txFile}
        "[razor-qt.${_tsSrcFileNameWE}]\n"
        "type = QT\n"
        "source_lang = en\n"
        "source_file = ${_tx_tsSrcFile}\n"
        "file_filter = ${_tx_translationDir}/${_tsSrcFileNameWE}_<lang>.ts\n"
        "\n"
    )

    # translate.h file *************************************
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/razortranslate.h
        "#ifndef RAZOR_TRANSLATE_H\n"
        "#include <QtCore/QLocale>\n"
        "#include <QtCore/QTranslator>\n"
        "#include <QtCore/QLibraryInfo>\n"
        "class RazorTranslator {\n"
        "public:\n"
        "  static void translate()\n"
        "  {\n"
        "    QString locale = QLocale::system().name();\n"

        "    QTranslator *qtTranslator = new QTranslator(qApp);\n"
        "    qtTranslator->load(\"qt_\" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));\n"
        "    qApp->installTranslator(qtTranslator);\n"

        "    QTranslator *appTranslator = new QTranslator(qApp);\n"
        "    appTranslator->load(QString(\"${_installDir}/${PROJECT_NAME}_%1.qm\").arg(locale));\n"
        "    qApp->installTranslator(appTranslator);\n"
        "  }\n"
        "};\n"

        "#define TRANSLATE_APP  RazorTranslator::translate();\n"
        "#endif // RAZOR_TRANSLATE_H\n"
    )

    # QM files **********************************************    
    file(GLOB _tsFiles ${_translationDir}/${_tsSrcFileNameWE}_*.ts)    
    QT4_ADD_TRANSLATION_FIXED(_qmFilesLocal ${_tsFiles})
    install(FILES ${_qmFilesLocal} DESTINATION ${_installDir})
    
    set(${_qmFiles} ${_qmFilesLocal} PARENT_SCOPE)
endfunction(razor_translate_ts)


#**********************************************************
# DESCTOP files
#**********************************************************

function(razor_translate_desktop2 _RESULT)
    set(_translationDir "translations")
    
    # Parse arguments ***************************************
    set(_state "")
    foreach (_arg ${ARGN})  
        if (
            ("${_arg}_I_HATE_CMAKE" STREQUAL "SOURCES_I_HATE_CMAKE") OR
            ("${_arg}_I_HATE_CMAKE" STREQUAL "TRANSLATION_DIR_I_HATE_CMAKE")
           )        

            set(_state ${_arg})
      
        else()
            if("${_state}" STREQUAL "SOURCES")
                get_filename_component (__file ${_arg} ABSOLUTE)
                set(_sources  ${_sources} ${__file})
                #set(_sources  ${_sources} ${_arg})
 
            elseif("${_state}" STREQUAL "TRANSLATION_DIR")
                set(_translationDir ${_arg})       
                set(_state "")

            else()  
                MESSAGE(FATAL_ERROR 
                  "Unknown argument '${_arg}'.\n"
                  "See ${CMAKE_CURRENT_LIST_FILE} for more information.\n"
                )
            endif()  
        endif()
    endforeach(_arg)    

    get_filename_component (_translationDir ${_translationDir} ABSOLUTE)    
    
    foreach (_inFile ${_sources})
        get_filename_component(_inFile   ${_inFile} ABSOLUTE)
        get_filename_component(_fileName ${_inFile} NAME_WE)
        #Extract the real extension ............
        get_filename_component(_fileExt  ${_inFile} EXT)
        string(REPLACE ".in" "" _fileExt ${_fileExt})
        #.......................................
        set(_outFile "${CMAKE_CURRENT_BINARY_DIR}/${_fileName}${_fileExt}")

        file(GLOB _translations
            ${_translationDir}/${_fileName}_*${_fileExt}
        )    
  
        set(_pattern "'\\[.*]\\s*='")
        if (_translations)
            add_custom_command(OUTPUT ${_outFile}
                COMMAND grep -v "'#TRANSLATIONS_DIR='" ${_inFile} > ${_outFile}
                COMMAND grep --no-filename ${_pattern} ${_translations} >> ${_outFile}
                COMMENT "Generating ${_fileName}${_fileExt}"
            )
        else()
            add_custom_command(OUTPUT ${_outFile}
                COMMAND grep -v "'#TRANSLATIONS_DIR='" ${_inFile} > ${_outFile}
                COMMENT "Generating ${_fileName}${_fileExt}"
            )
        endif()

        set(__result ${__result} ${_outFile}) 


        # TX file ***********************************************
        set(_txFile "${CMAKE_BINARY_DIR}/tx/${_fileName}${_fileExt}.tx_config")  
        string(REPLACE "${CMAKE_SOURCE_DIR}/" "" _tx_translationDir ${_translationDir})
        string(REPLACE "${CMAKE_SOURCE_DIR}/" "" _tx_inFile ${_inFile})
    
        file(WRITE ${_txFile}
            "[razor-qt.${_fileName}_desktop]\n"
            "type = DESKTOP\n"
            "source_lang = en\n"
            "source_file = ${_tx_inFile}\n"
            "file_filter = ${_tx_translationDir}/${_fileName}_<lang>${_fileExt}\n"
            "\n"
        )

    endforeach()

    set(${_RESULT} ${__result} PARENT_SCOPE)    
endfunction(razor_translate_desktop2)





#**********************************************************
# TS files
#**********************************************************

macro(razor_translate_to _QM_FILES _TRANSLATIONS_DIR)
    file(GLOB TS_FILES
        translations/*.ts
        translations/local/*.ts
    )

    set(TRANSLATIONS_DIR ${_TRANSLATIONS_DIR})
    add_definitions(-DTRANSLATIONS_DIR=\"${TRANSLATIONS_DIR}\")
    QT4_ADD_TRANSLATION_FIXED(${_QM_FILES} ${TS_FILES})

    install(FILES ${${_QM_FILES}} DESTINATION ${TRANSLATIONS_DIR})

    configure_file(${CMAKE_MODULE_PATH}/razortranslate.h.in razortranslate.h)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endmacro(razor_translate_to)


macro(razor_translate _QM_FILES)
    razor_translate_to(${_QM_FILES} ${CMAKE_INSTALL_PREFIX}/share/razor/${PROJECT_NAME})
endmacro(razor_translate)


#**********************************************************
# DESCTOP files
#**********************************************************

macro(razor_translate_desktop _RESULT)
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

    #if (NOT TR_FILES)
    #    message(WARNING "Translations files not found for ${_current_FILE}")
    #endif()

    #message(status ".:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:")
    #message(status "EXT ${_FILE_EXT}")
    #message(status "IN:     ${_IN_FILE}")
    #message(status "OUT:    ${_OUT_FILE}")
    #message(status "TR Dir: ${CMAKE_CURRENT_SOURCE_DIR}")
    #message(status "TR:  ${TR_FILES}")
    #message(status ".:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:")

    set(PATTERN "'\\[.*]\\s*='")
    if (TR_FILES)
        add_custom_command(OUTPUT ${_OUT_FILE}
            COMMAND grep -v "'#TRANSLATIONS_DIR='" ${_IN_FILE} > ${_OUT_FILE}
            COMMAND grep --no-filename ${PATTERN} ${TR_FILES} >> ${_OUT_FILE}
            COMMENT "Generating ${_FILE_NAME}${_FILE_EXT}"
        )
    else()
        add_custom_command(OUTPUT ${_OUT_FILE}
            COMMAND grep -v "'#TRANSLATIONS_DIR='" ${_IN_FILE} > ${_OUT_FILE}
            COMMENT "Generating ${_FILE_NAME}${_FILE_EXT}"
        )
    endif()

    set(${_RESULT} ${${_RESULT}} ${_OUT_FILE})
  endforeach(_current_FILE)
endmacro(razor_translate_desktop)



