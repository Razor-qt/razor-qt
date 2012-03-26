set(PROJECT razor-notificationd)
project( ${PROJECT} )
include_directories(../libraries)

set( QT_USE_QTDECLARATIVE 1 )
set( QT_USE_QTDBUS 1 )

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})



add_subdirectory(service)
include_directories(service/)

set( razorqtnotify_srcs
    src/notification.cpp
    src/notificationhandler.cpp
    src/notificationtimeout.cpp
    src/inotificationview.cpp
    src/notificationserversettings.cpp
    src/debug_helpers.cpp
    src/main.cpp
    src/qtndbusconnector.cpp
)

set( razorqtnotify_mocs
    src/notificationhandler.h
    src/notificationtimeout.h
    src/inotificationview.h
    src/notificationserversettings.h
)

set( razorqtnotify_hdrs
    src/qtnlog.h
    src/notification.h
)

set( widgetnotification_rsc
    images.qrc
)

include_directories(src/)

add_subdirectory(src/qmlnotification)
add_subdirectory(src/widgetnotification)


set(libraries
    ${QT_LIBRARIES}
    qmlnotifications
    widgetnotification
    razorqt
    qtservice
)


set( razorqtnotify_srcs ${razorqtnotify_srcs} )
set(razorqtnotify_mocs ${razorqtnotify_mocs} src/qtndbusconnector.h )
set( libraries ${libraries}  )

qt4_wrap_cpp(razorqtnotify_mocs ${razorqtnotify_mocs})
#qt4_add_resources(razorqtnotify_rsc_src ${widgetnotification_rsc})
add_executable( razorqt-notificationd ${razorqtnotify_srcs}  ${razorqtnotify_mocs} ${razorqtnotify_mocs} ${qtnotification_rcc} ${razorqtnotify_hdrs})
target_link_libraries( razorqt-notificationd ${libraries} )

# file(COPY NotificationUI DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(clientlibrary)
