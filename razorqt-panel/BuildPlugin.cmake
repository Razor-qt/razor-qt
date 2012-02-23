MACRO (BUILD_RAZOR_PLUGIN Name)
    project(${Name})
    set(PROGRAM "razor-panel")

    set(PROG_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/razor/${PROGRAM})
    set(PLUGIN_SHARE_DIR ${PROG_SHARE_DIR}/${Name})

    # Translations **********************************
    include(RazorTranslate)
    razor_translate_to(QM_FILES ${CMAKE_INSTALL_PREFIX}/share/razor/${PROGRAM}/${PROJECT_NAME})
    file (GLOB DESKTOP_FILES_IN resources/*.desktop.in)
    razor_translate_desktop(DESKTOP_FILES ${DESKTOP_FILES_IN})
    #************************************************

    file (GLOB CONFIG_FILES     resources/*.conf    )

    include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries
        ${CMAKE_CURRENT_SOURCE_DIR}/panel
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    if (NOT DEFINED PLUGIN_DIR)
        set (PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/${PROGRAM}/)
    endif (NOT DEFINED PLUGIN_DIR)

    find_package(Qt4 REQUIRED)
    include(${QT_USE_FILE})
    qt4_wrap_cpp(MOC_SOURCES ${MOCS})
    qt4_add_resources(QRC_SOURCES ${RESOURCES})
    qt4_wrap_ui(UI_SOURCES ${UIS})

    add_library(${Name} MODULE ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QM_FILES} ${QRC_SOURCES} ${UIS} ${DESKTOP_FILES})
    target_link_libraries(${Name} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${LIBRARIES})

    install(TARGETS ${Name} DESTINATION ${PLUGIN_DIR})
    install(FILES ${CONFIG_FILES}  DESTINATION ${PLUGIN_SHARE_DIR})
    install(FILES ${DESKTOP_FILES} DESTINATION ${PROG_SHARE_DIR})

    add_dependencies(${Name} razorqt)
ENDMACRO(BUILD_RAZOR_PLUGIN)



