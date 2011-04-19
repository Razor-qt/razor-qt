MACRO (BUILD_RAZOR_PLUGIN Name)
    set(PROJECT "razorpanel_${Name}")

    project(${PROJECT})

    # Translations .............. 
    set(TRANSLATIONS_DIR 
        ${CMAKE_INSTALL_PREFIX}/share/razor/razor-panel
    )

    add_definitions(-DTRANSLATIONS_DIR=\"${TRANSLATIONS_DIR}\")

    file (GLOB TS_FILES         translations/*.ts   )
    file (GLOB CONFIG_FILES     resources/*.conf    )
    file (GLOB DESKTOP_FILES    resources/*.desktop )

    include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/../../librazorqt
        ${CMAKE_CURRENT_SOURCE_DIR}/panel
    )

    if (NOT DEFINED PLUGIN_DIR)
        set (PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-panel/)
    endif (NOT DEFINED PLUGIN_DIR)

    find_package(Qt4 REQUIRED)
    include(${QT_USE_FILE})
    qt4_wrap_cpp(MOC_SOURCES ${MOCS})
    qt4_add_resources(QRC_SOURCES ${RESOURCES})
    qt4_add_translation(QM_FILES ${TS_FILES})

    add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES})
    target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${LIBRARIES})

    install(TARGETS ${PROJECT} DESTINATION ${PLUGIN_DIR})
    install(FILES ${QM_FILES} DESTINATION ${TRANSLATIONS_DIR})
    install(FILES ${CONFIG_FILES} DESTINATION share/razor)
    install(FILES ${DESKTOP_FILES} DESTINATION share/razor/razor-panel/plugins)
  
ENDMACRO(BUILD_RAZOR_PLUGIN)



