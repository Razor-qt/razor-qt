set(PLUGIN "quicklaunch")

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

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../../librazorqt
    ${CMAKE_CURRENT_SOURCE_DIR}/../panel
)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
