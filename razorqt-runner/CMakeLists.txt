set(PROJECT razor-runner)
project(${PROJECT})


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
    qtxdg
)

set(QT_USE_QTXML true)
set(QT_USE_QTSCRIPT true)


macro( setByDefault VAR_NAME VAR_VALUE )
  if (NOT DEFINED ${VAR_NAME})
    set (${VAR_NAME} ${VAR_VALUE})
  endif (NOT DEFINED ${VAR_NAME})
endmacro()


# *******************************************************************
# What plugins will be built, by default.
# You can enable/disable building of the plugin using cmake options.
#    cmake -DRUNNER_MATH=Yes .. # Enable math plugin
#    cmake -DRUNNER_MATH=No ..  # Disable math plugin

setByDefault(RUNNER_MATH Yes )
setByDefault(RUNNER_VBOX Yes )


# Translations **********************************
include(RazorTranslate)
razor_translate(QM_FILES)
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
set(APP_SHARE_DIR ${RAZOR_SHARE_DIR}/${PROJECT})

#************************************************

if (RUNNER_MATH)
    add_definitions(-DMATH_ENABLED)
endif (RUNNER_MATH)

if (RUNNER_VBOX)
	set(QRC_FILES 
		${QRC_FILES}
		virtualbox-icons.qrc
	)
	add_definitions(-DVBOX_ENABLED)
endif (RUNNER_VBOX)

qt4_wrap_cpp(MOC_SOURCES ${MOC_FILES})
qt4_wrap_ui(UI_HEADERS ${UI_FILES})
qt4_add_resources(QRC_SOURCES ${QRC_FILES})

add_executable(${PROJECT} ${CPP_FILES} ${UI_FILES} ${RESOURCES} ${QRC_SOURCES} ${QM_FILES} ${MOC_SOURCES})
add_dependencies(${PROJECT} razorqt)
target_link_libraries(${PROJECT}  ${LIBRARIES} ${QT_LIBRARIES})

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES   ${CONFIG_FILES}    DESTINATION ${APP_SHARE_DIR})
