project( qtnotificationdeamon2 )
cmake_minimum_required( VERSION 2.8 )

set(QT_MIN_VERSION 4.7.0)

find_package( Qt4 REQUIRED )
set( QT_USE_QTDECLARATIVE 1 )

option(TEST_BUILD "Provide test build instead of real one" OFF )
if(TEST_BUILD)
    set( QT_USE_QTDBUS 0 )
else()
    set ( QT_USE_QTDBUS 1 )
endif()

include (${QT_USE_FILE})
if(NOT TEST_BUILD)
    add_subdirectory(service)
    include_directories(service/)
endif()

set( razorqtnotify_srcs
    src/notification.cpp
    src/notificationhandler.cpp
    src/notificationtimeout.cpp
    src/inotificationview.cpp
    src/notificationserversettings.cpp
    src/debug_helpers.cpp
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

include_directories(src/)

add_subdirectory(src/qmlnotification)
add_subdirectory(src/widgetnotification)


set(libraries
    ${QT_LIBRARIES}
    qmlnotifications
    widgetnotification
)



if(TEST_BUILD)
    set(razorqtnotify_srcs ${razorqtnotify_srcs} src/test_main.cpp src/testerapp.cpp)
    set(razorqtnotify_mocs ${razorqtnotify_mocs} src/testerapp.h)
    add_definitions(-DTEST_BUILD)
else()
    set( razorqtnotify_srcs ${razorqtnotify_srcs} src/main.cpp src/qtndbusconnector.cpp)
    set(razorqtnotify_mocs ${razorqtnotify_mocs} src/qtndbusconnector.h )
    set( libraries ${libraries} qtservice )

endif()

qt4_wrap_cpp(razorqtnotify_mocs ${razorqtnotify_mocs})
add_executable( razorqt-notifyd ${razorqtnotify_srcs}  ${razorqtnotify_mocs} ${razorqtnotify_mocs} ${qtnotification_rcc} ${razorqtnotify_hdrs})
target_link_libraries( razorqt-notifyd ${libraries} )

# file(COPY NotificationUI DESTINATION ${CMAKE_BINARY_DIR})
