project(razor-config-appearance)

include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/
    )

set(H_FILES
    iconthemeinfo.h
   )

set(MOC_FILES
    iconthemeconfig.h
    razorthemeconfig.h
)

set(CPP_FILES
    main.cpp
    iconthemeconfig.cpp
    iconthemeinfo.cpp
    razorthemeconfig.cpp
   )

set(UI_FILES
    iconthemeconfig.ui
    razorthemeconfig.ui
)

set(QRC_FILES

)

set(LIBRARIES
    razorqt
    qtxdg
)


# Translations **********************************
include(RazorTranslate)
razor_translate_ts(QM_FILES 
    SOURCES
        ${H_FILES}
        ${CPP_FILES} 
        ${UI_FILES}
    INSTALLATION_DIR
        ${CMAKE_INSTALL_PREFIX}/share/razor/razor-config
)

razor_translate_desktop(DESKTOP_FILES
    SOURCES
        ${PROJECT_NAME}.desktop.in
) 

#************************************************

qt4_wrap_cpp(MOC_SOURCES ${MOC_FILES})
qt4_wrap_ui(UI_HEADERS ${UI_FILES})
qt4_add_resources(QRC_SOURCES ${QRC_FILES})


add_executable(${PROJECT_NAME} ${CPP_FILES} ${UI_FILES} ${RESOURCES} ${QRC_SOURCES} ${QM_FILES} ${MOC_SOURCES} ${DESKTOP_FILES})
add_dependencies(${PROJECT_NAME} razorqt qtxdg)
target_link_libraries(${PROJECT_NAME}  ${LIBRARIES} ${QT_LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
