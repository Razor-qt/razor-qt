
set( widgetnotification_src
    widgetnotification.cpp
    widgetnotificationnotifier.cpp
)

set( widgetnotification_moc
    widgetnotification.h
    widgetnotificationnotifier.h
)

set( widgetnotification_ui
    ui/notification.ui
    ui/normalNotification.ui
    ui/notificationWithProgress.ui
)

set( widgetnotification_rsc
    images.qrc
)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ../../../libraries/razorqt)

qt4_wrap_ui( widgetnotification_ui_srcs ${widgetnotification_ui} )
qt4_wrap_cpp(widgetnotification_moc_src ${widgetnotification_moc})
qt4_add_resources(widgetnotification_rsc_src ${widgetnotification_rsc})
add_library( widgetnotification STATIC ${widgetnotification_src} ${widgetnotification_moc_src} ${widgetnotification_ui_srcs} ${widgetnotification_rsc_src})

