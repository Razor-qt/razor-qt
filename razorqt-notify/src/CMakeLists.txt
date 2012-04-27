include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/razorqt
    ${CMAKE_SOURCE_DIR}/libraries/qtxdg
    ${CMAKE_SOURCE_DIR}/libraries/qtservice
)

set(razorqtnotify_srcs
    notification.cpp
    notificationhandler.cpp
    notificationtimeout.cpp
    debug_helpers.cpp
    main.cpp
    qtndbusconnector.cpp
)

set(razorqtnotify_mocs
    notificationhandler.h
    notificationtimeout.h
)

set(razorqtnotify_hdrs
    qtnlog.h
    notification.h
    inotificationview.h
)

set(razorqtnotify_rsc
    ../images/images.qrc
)

add_subdirectory(qmlnotification)
add_subdirectory(widgetnotification)

set(libraries
    ${QT_LIBRARIES}
    qmlnotifications
    widgetnotification
    razorqt
    qtservice
)

set(razorqtnotify_srcs ${razorqtnotify_srcs})
set(razorqtnotify_mocs ${razorqtnotify_mocs} qtndbusconnector.h)
set(libraries ${libraries})

qt4_wrap_cpp(razorqtnotify_mocs ${razorqtnotify_mocs})
qt4_add_resources(razorqtnotify_rsc_src ${razorqtnotify_rsc})

add_executable(razor-notificationd
    ${razorqtnotify_srcs}
    ${razorqtnotify_mocs}
    ${qtnotification_rcc}
    ${razorqtnotify_rsc_src}
)
target_link_libraries(razor-notificationd ${libraries})

INSTALL(TARGETS razor-notificationd RUNTIME DESTINATION bin)
