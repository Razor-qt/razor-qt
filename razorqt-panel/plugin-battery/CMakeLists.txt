find_package ( QtMobility )
message(STATUS "QT_MOBILITY_SYSTEMINFO_INCLUDE_DIR: ${QT_MOBILITY_SYSTEMINFO_INCLUDE_DIR}")
message(STATUS "QT_MOBILITY_SYSTEMINFO_LIBRARY: ${QT_MOBILITY_SYSTEMINFO_LIBRARY}")

if (NOT QT_MOBILITY_FOUND)
    message(STATUS "")
    message(STATUS "Battery panel plugin cannot be build - no Qt Mobility lib found")
    message(STATUS "")
else (NOT QT_MOBILITY_FOUND)
    message(STATUS "Battery panel plugin will be build. Qt Mobility found.")
    include_directories(${QT_MOBILITY_SYSTEMINFO_INCLUDE_DIR}
                        ${QT_MOBILITY_INCLUDE_DIR}
                        ${QT_INCLUDE_DIR}/QtNetwork)
    QT4_WRAP_CPP(battery_moc battery.h)
    QT4_ADD_RESOURCES(battery_qrc battery.qrc)
    add_library(razorpanel_battery SHARED ${battery_moc} ${battery_qrc} battery.cpp)
    target_link_libraries(razorpanel_battery ${QT_QTCORE_LIBRARY}
                                             ${QT_QTGUI_LIBRARY}
                                             ${QT_MOBILITY_SYSTEMINFO_LIBRARY})
    install(TARGETS razorpanel_battery DESTINATION ${PLUGIN_DIR})
endif (NOT QT_MOBILITY_FOUND)
