
set( qmlnotification_src
    notificationview.cpp
    ../debug_helpers.cpp
)

set( qmlnotification_moc
    notificationview.h
)

qt4_wrap_cpp(qmlnotification_moc_src ${qmlnotification_moc})
add_library ( qmlnotifications SHARED ${qmlnotification_src} ${qmlnotification_moc_src} )

file(COPY NotificationUI DESTINATION ${CMAKE_BINARY_DIR})
