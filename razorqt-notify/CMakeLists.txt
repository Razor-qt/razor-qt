set(PROJECT razor-notificationd)
project( ${PROJECT} )
include_directories(../libraries)

set( QT_USE_QTDECLARATIVE 1 )
set( QT_USE_QTDBUS 1 )

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

set( razorqtnotify_srcs
    src/notification.cpp
    src/notificationhandler.cpp
    src/notificationtimeout.cpp
    src/debug_helpers.cpp
    src/main.cpp
    src/qtndbusconnector.cpp
)

set( razorqtnotify_mocs
    src/notificationhandler.h
    src/notificationtimeout.h
)

set( razorqtnotify_hdrs
    src/qtnlog.h
    src/notification.h
    src/inotificationview.h
)

set( razorqtnotify_rsc
    images/images.qrc
)

include_directories(src/ ../libraries/razorqt ../libraries/qtxdg ../libraries/qtservice)

add_subdirectory(src/qmlnotification)
add_subdirectory(src/widgetnotification)

set(libraries
    ${QT_LIBRARIES}
    qmlnotifications
    widgetnotification
    razorqt
    qtservice
)


set(razorqtnotify_srcs ${razorqtnotify_srcs} )
set(razorqtnotify_mocs ${razorqtnotify_mocs} src/qtndbusconnector.h )
set(libraries ${libraries}  )

qt4_wrap_cpp(razorqtnotify_mocs ${razorqtnotify_mocs})
qt4_add_resources(razorqtnotify_rsc_src ${razorqtnotify_rsc})
add_executable( razor-notificationd ${razorqtnotify_srcs}  ${razorqtnotify_mocs} ${razorqtnotify_mocs} ${qtnotification_rcc} ${razorqtnotify_hdrs} ${razorqtnotify_rsc_src})
target_link_libraries( razor-notificationd ${libraries} )

INSTALL(TARGETS razor-notificationd RUNTIME DESTINATION bin)
