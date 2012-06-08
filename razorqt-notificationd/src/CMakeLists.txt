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
    notifyadaptor.cpp
    notifyd.cpp
)

set(NOTIFICATIONS_MOC
    notificationarea.h
    notification.h
    notificationlayout.h
    notificationwidgets.h
    notifyadaptor.h
    notifyd.h
)

set(NOTIFICATIONS_UI
    notification.ui
)

qt4_wrap_cpp(NOTIFICATIONS_MOC_CPP ${NOTIFICATIONS_MOC})
qt4_wrap_ui(NOTIFICATIONS_UI_CPP ${NOTIFICATIONS_UI})

add_executable(razor-notificationd
    ${NOTIFICATIONS_SRC}
    ${NOTIFICATIONS_MOC_CPP}
    ${NOTIFICATIONS_UI_CPP}
)
add_dependencies(razor-notificationd razorqt)
target_link_libraries(razor-notificationd ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                                 ${QT_QTDBUS_LIBRARY}
                                 razorqt qtxdg
                     )

INSTALL(TARGETS razor-notificationd RUNTIME DESTINATION bin)
