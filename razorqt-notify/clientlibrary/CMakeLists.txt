set(PROJECT razor-notifications)
project( ${PROJECT} )
set( QT_USE_QTDBUS 1 )

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

set( razornotifications_srcs
    razornotification.cpp
)

set( razornotifications_hdrs
    razornotification.h
)

set(libraries
    ${QT_LIBRARIES}
)

add_library( razor-notifications ${razornotifications_srcs} ${razornotifications_mocs} ${razornotifications_hdrs})
target_link_libraries( razor-notifications ${libraries} )

