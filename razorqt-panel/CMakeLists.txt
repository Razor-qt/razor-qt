project( razor-panel )
cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)


macro( setByDefault VAR_NAME VAR_VALUE )
  if (NOT DEFINED ${VAR_NAME})
    set (${VAR_NAME} ${VAR_VALUE})
  endif (NOT DEFINED ${VAR_NAME})
endmacro()


# *******************************************************************
# What plugins will be built, by default.
# You can enable/disable building of the plugin using cmake options.
#    cmake -DCLOCK_PLUGIN=Yes .. # Enable clock plugin
#    cmake -DCLOCK_PLUGIN=No ..  # Disable clock plugin

#setByDefault(DEV_PLUGIN         Yes )
setByDefault(CLOCK_PLUGIN       Yes )
setByDefault(DESKTOPSWITCH_PLUGIN  Yes )
#setByDefault(LOGOUTMENU_PLUGIN  Yes )
setByDefault(QUICKLAUNCH_PLUGIN Yes )
#setByDefault(SPINBUTTON_PLUGIN  Yes )
setByDefault(TRAY_PLUGIN        Yes )
setByDefault(MAINMENU_PLUGIN    Yes )
#setByDefault(COMMAND_PLUGIN     Yes )
setByDefault(TASKBAR_PLUGIN     Yes )
#setByDefault(SPACER_PLUGIN      Yes )
#setByDefault(BATTERY_PLUGIN     No  )

# *******************************************************************



include(RazorLibSuffix)


add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( X11 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR}
        ${X11_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/librazorqt/
	.
	src)

set (PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-panel/)
add_definitions(-DPLUGIN_DIR=\"${PLUGIN_DIR}\")
message(STATUS "Panel plugins location: ${PLUGIN_DIR}")


add_subdirectory(panel)


set(ENABLED_PLUGINS)
#dev plugin
# if (DEV_PLUGIN)
#   set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Dev")
#   QT4_WRAP_CPP(devplugin_moc plugin-devplugin/razordevplugin.h)
#   add_library(razorpanel_devplugin SHARED ${devplugin_moc} plugin-devplugin/razordevplugin.cpp)
#   target_link_libraries(razorpanel_devplugin ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
#   install(TARGETS razorpanel_devplugin DESTINATION ${PLUGIN_DIR})
# endif (DEV_PLUGIN)


if (CLOCK_PLUGIN)
  set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Clock")
  add_subdirectory( plugin-clock )
endif (CLOCK_PLUGIN)


if (DESKTOPSWITCH_PLUGIN)
  set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "DesktopSwitch")
  add_subdirectory( plugin-desktopswitch )
endif (DESKTOPSWITCH_PLUGIN)


# if (LOGOUTMENU_PLUGIN)
#   set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Logout menu")
#   QT4_WRAP_CPP(logoutmenu_moc plugin-logoutmenu/razorlogoutmenu.h)
#   add_library(razorpanel_logoutmenu SHARED ${logoutmenu_moc} plugin-logoutmenu/razorlogoutmenu.cpp)
#   target_link_libraries(razorpanel_logoutmenu ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
#   install(TARGETS razorpanel_logoutmenu DESTINATION ${PLUGIN_DIR})
# endif (LOGOUTMENU_PLUGIN)


if (QUICKLAUNCH_PLUGIN)
  set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Quicklaunch")
  add_subdirectory( plugin-quicklaunch )
endif (QUICKLAUNCH_PLUGIN)


# if (SPINBUTTON_PLUGIN)
#   set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Spin button")
#   QT4_WRAP_CPP(spinbutton_moc plugin-spinbutton/razorspinbutton.h)
#   add_library(razorpanel_razorspinbutton SHARED ${spinbutton_moc} plugin-spinbutton/razorspinbutton.cpp)
#   target_link_libraries(razorpanel_razorspinbutton ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
#   install(TARGETS razorpanel_razorspinbutton DESTINATION ${PLUGIN_DIR})
# endif (SPINBUTTON_PLUGIN)


if (TRAY_PLUGIN)
  set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "System Tray")
  add_subdirectory( plugin-tray )
endif (TRAY_PLUGIN)


if (MAINMENU_PLUGIN)
  set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Main menu")
  add_subdirectory( plugin-mainmenu )
endif (MAINMENU_PLUGIN)
#
#
# if (COMMAND_PLUGIN)
#   set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Command")
#   QT4_WRAP_CPP(cmd_moc plugin-command/razorcmd.h)
#   add_library(razorpanel_cmd SHARED ${cmd_moc} plugin-command/razorcmd.cpp)
#   target_link_libraries(razorpanel_cmd ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
#   install(TARGETS razorpanel_cmd DESTINATION ${PLUGIN_DIR})
# endif (COMMAND_PLUGIN)


if (TASKBAR_PLUGIN)
  set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Taskbar")
  add_subdirectory( plugin-taskbar )
endif (TASKBAR_PLUGIN)


# if (SPACER_PLUGIN)
#   set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Spacer")
#   QT4_WRAP_CPP(spacer_moc plugin-spacer/spacer.h)
#   add_library(razorpanel_spacer SHARED ${spacer_moc} plugin-spacer/spacer.cpp)
#   target_link_libraries(razorpanel_spacer ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
#   install(TARGETS razorpanel_spacer DESTINATION ${PLUGIN_DIR})
# endif (SPACER_PLUGIN)
#
#
# if (BATTERY_PLUGIN)
#   set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Battery")
#   add_subdirectory( plugin-battery )
# endif (BATTERY_PLUGIN)
#

message(STATUS  "**************** The following plugins will be built ****************")
foreach (PLUGIN_STR ${ENABLED_PLUGINS})
  message(STATUS   "  ${PLUGIN_STR}")
endforeach()
message(STATUS  "*********************************************************************")

