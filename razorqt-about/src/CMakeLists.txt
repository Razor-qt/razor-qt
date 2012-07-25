project(razor-about)


set(razor-about_H_FILES
)

set(razor-about_MOC_FILES
)

set(razor-about_CPP_FILES
    main.cpp
)


set(razor-about_UI_FILES
)

set(razor-about_QRC_FILES

)

set(razor-about_LIBRARIES
    razorqt
    qtxdg
)

# Translations **********************************
include(RazorTranslate)
razor_translate_ts(razor-runner_QM_FILES 
    SOURCES
        ${razor-about_H_FILES}
        ${razor-about_CPP_FILES} 
        ${razor-about_UI_FILES}
)

razor_translate_desktop2(razor-about_DESKTOP_FILES
    SOURCES
        resources/razor-about.desktop.in
)
#************************************************
cmake_minimum_required( VERSION 2.6 )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/
)


if(NOT CMAKE_BUILD_TYPE)
    set( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions(-Wall)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})


set(RAZOR_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/razor)
set(APP_SHARE_DIR ${RAZOR_SHARE_DIR}/${PROJECT_NAME})

#************************************************


qt4_wrap_cpp(razor-about_MOC_SOURCES ${razor-about_MOC_FILES})
qt4_wrap_ui(razor-about_UI_HEADERS ${razor-about_UI_FILES})
qt4_add_resources(razor-about_QRC_SOURCES ${razor-about_QRC_FILES})

add_executable(${PROJECT_NAME} 
    ${razor-about_CPP_FILES} 
    ${razor-about_UI_FILES} 
    ${razor-about_RESOURCES} 
    ${razor-about_QRC_SOURCES} 
    ${razor-about_QM_FILES} 
    ${razor-about_MOC_SOURCES} 
    ${razor-about_DESKTOP_FILES}
)
add_dependencies(${PROJECT_NAME} razorqt)
target_link_libraries(${PROJECT_NAME}  ${razor-about_LIBRARIES} ${QT_LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES   ${CONFIG_FILES}    DESTINATION ${APP_SHARE_DIR})
install(FILES   ${razor-about_DESKTOP_FILES}   DESTINATION share/applications)
