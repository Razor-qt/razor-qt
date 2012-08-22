project(razor-runner)


set(razor-runner_H_FILES
    dialog.h
    commanditemmodel.h
    widgets.h
    providers.h
    configuredialog/configuredialog.h
)

set(razor-runner_MOC_FILES
    dialog.h
    commanditemmodel.h
    widgets.h
    configuredialog/configuredialog.h
    providers.h
)

set(razor-runner_CPP_FILES
    main.cpp
    dialog.cpp
    commanditemmodel.cpp
    widgets.cpp
    providers.cpp
    configuredialog/configuredialog.cpp
)

set(razor-runner_UI_FILES
    dialog.ui
    configuredialog/configuredialog.ui
)

set(QRC_FILES

)

set(razor-runner_LIBRARIES
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
razor_translate_ts(razor-runner_QM_FILES 
    SOURCES
        ${razor-runner_H_FILES}
        ${razor-runner_CPP_FILES} 
        ${razor-runner_UI_FILES}
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
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

set(RAZOR_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/razor)
set(APP_SHARE_DIR ${RAZOR_SHARE_DIR}/${PROJECT_NAME})

#************************************************

if (RUNNER_MATH)
    add_definitions(-DMATH_ENABLED)
endif (RUNNER_MATH)

# *******************************************************************
# Optional: start VBOX machines in headless mode,
# instead of default window
if (VBOXRUNNER_HEADLESS)
	add_definitions(-DVBOXRUNNER_HEADLESS)
endif (VBOXRUNNER_HEADLESS)

if (RUNNER_VBOX)
	set(QRC_FILES 
		${QRC_FILES}
		virtualbox-icons.qrc
	)
	add_definitions(-DVBOX_ENABLED)
endif (RUNNER_VBOX)

qt4_wrap_cpp(razor-runner_MOC_SOURCES ${razor-runner_MOC_FILES})
qt4_wrap_ui(razor-runner_UI_HEADERS ${razor-runner_UI_FILES})
qt4_add_resources(razor-runner_QRC_SOURCES ${QRC_FILES})

add_executable(${PROJECT_NAME} 
    ${razor-runner_CPP_FILES} 
    ${razor-runner_UI_FILES} 
    ${razor-runner_QRC_SOURCES} 
    ${razor-runner_QM_FILES} 
    ${razor-runner_MOC_SOURCES})

add_dependencies(${PROJECT_NAME} razorqt)
target_link_libraries(${PROJECT_NAME}  ${razor-runner_LIBRARIES} ${QT_LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES   ${CONFIG_FILES}    DESTINATION ${APP_SHARE_DIR})
