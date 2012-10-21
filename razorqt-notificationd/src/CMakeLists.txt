include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/
)

set(NOTIFICATIONS_SRC
    main.cpp
    notificationarea.cpp
    notification.cpp
    notificationlayout.cpp
    notificationwidgets.cpp
    notifyd.cpp
)

set(NOTIFICATIONS_MOC
    notificationarea.h
    notification.h
    notificationlayout.h
    notificationwidgets.h
    notifyd.h
)

set(NOTIFICATIONS_UI
    notification.ui
)

qt4_wrap_cpp(NOTIFICATIONS_MOC_CPP ${NOTIFICATIONS_MOC})
qt4_wrap_ui(NOTIFICATIONS_UI_CPP ${NOTIFICATIONS_UI})
qt4_add_dbus_adaptor(NOTIFICATIONS_SRC
    ../../razorqt-resources/dbus/org.freedesktop.Notifications.xml
    notifyd.h Notifyd
)

# Translations **********************************
include(RazorTranslate)
razor_translate_ts(NOTIFICATIONS_QM_FILES
    SOURCES
        ${NOTIFICATIONS_MOC}
        ${NOTIFICATIONS_SRC}
        ${NOTIFICATIONS_UI}
)
#************************************************

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

add_executable(razor-notificationd
    ${NOTIFICATIONS_SRC}
    ${NOTIFICATIONS_MOC_CPP}
    ${NOTIFICATIONS_UI_CPP}
    ${NOTIFICATIONS_QM_FILES}
)
add_dependencies(razor-notificationd razorqt)
target_link_libraries(razor-notificationd ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                                 ${QT_QTDBUS_LIBRARY}
                                 razorqt qtxdg
                     )

INSTALL(TARGETS razor-notificationd RUNTIME DESTINATION bin)
