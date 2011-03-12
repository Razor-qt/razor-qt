set(PLUGIN "quicklaunch2")

# if (QUICKLAUNCH_PLUGIN)
#   set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Quick launch")
#   QT4_WRAP_CPP(quicklaunch_moc plugin-quicklaunch/razorquicklaunch.h)
#   add_library(razorpanel_quicklaunch SHARED ${quicklaunch_moc} plugin-quicklaunch/razorquicklaunch.cpp)
#   target_link_libraries(razorpanel_quicklaunch ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
#   install(TARGETS razorpanel_quicklaunch DESTINATION ${PLUGIN_DIR})
# endif (QUICKLAUNCH_PLUGIN)
set(HEADERS
  razorquicklaunch.h
)

set(SOURCES
  razorquicklaunch.cpp
)

set(MOCS
  razorquicklaunch.h
)

#set(LIBRARIES
#  razorqt
#)

# QRC files .................
set(RESOURCES
)


file (GLOB CONFIG_FILES
  resources/*.conf
)

#*******************************************
set(PROJECT "razorpanel_${PLUGIN}")

project(${PROJECT})

# Translations ..............
set(TRANSLATIONS_DIR
    ${CMAKE_INSTALL_PREFIX}/share/razor/razor-panel
)

add_definitions(-DTRANSLATIONS_DIR=\"${TRANSLATIONS_DIR}\")

file (GLOB TS_FILES
    translations/*.ts
)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../../librazorqt
    ${CMAKE_CURRENT_SOURCE_DIR}/../panel
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
target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS ${PROJECT} DESTINATION ${PLUGIN_DIR})
install(FILES ${CONFIG_FILES} DESTINATION share/razor)
install(FILES ${QM_FILES} DESTINATION ${TRANSLATIONS_DIR}2)


