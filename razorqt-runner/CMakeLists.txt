set(PROJECT razor-runner)

set(H_FILES
    dialog.h
    commanditemmodel.h
    widgets.h
    providers.h
    configuredialog/configuredialog.h
)

set(MOC_FILES
    dialog.h
    commanditemmodel.h
    widgets.h
    configuredialog/configuredialog.h
)

set(CPP_FILES
    main.cpp
    dialog.cpp
    commanditemmodel.cpp
    widgets.cpp
    providers.cpp
    configuredialog/configuredialog.cpp
)

set(UI_FILES
    dialog.ui
    configuredialog/configuredialog.ui
)

set(QRC_FILES

)

set(LIBRARIES
    razorqt
    razorqxt
)

set(QT_USE_QTXML 1)

set(THEMES
    resources/themes
)

if (RUNNER_MATH)
    add_definitions(-DMATH_ENABLED)
endif (RUNNER_MATH)


# Translations **********************************
file(GLOB TS_FILES
    translations/*.ts
)
#************************************************
cmake_minimum_required( VERSION 2.6 )

include_directories(
    .
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/librazorqt/
)


if(NOT CMAKE_BUILD_TYPE)
    set( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions(-Wall)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})


set(RAZOR_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/razor)
set(APP_SHARE_DIR ${RAZOR_SHARE_DIR}/${PROJECT})
add_definitions(-DTRANSLATIONS_DIR=\"${APP_SHARE_DIR}\")

#************************************************


qt4_wrap_cpp(MOC_SOURCES ${MOC_FILES})
qt4_wrap_ui(UI_HEADERS ${UI_FILES})
qt4_add_resources(QRC_SOURCES ${QRC_FILES})
qt4_add_translation(QM_FILES ${TS_FILES})

add_executable(${PROJECT} ${CPP_FILES} ${UI_FILES} ${RESOURCES} ${QRC_SOURCES} ${QM_FILES} ${MOC_SOURCES})
add_dependencies(${PROJECT} razorqt)
target_link_libraries(${PROJECT}  ${LIBRARIES} ${QT_LIBRARIES})

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES   ${QM_FILES}        DESTINATION ${APP_SHARE_DIR})
install(FILES   ${CONFIG_FILES}    DESTINATION ${APP_SHARE_DIR})
install(DIRECTORY ${THEMES}        DESTINATION ${RAZOR_SHARE_DIR})
