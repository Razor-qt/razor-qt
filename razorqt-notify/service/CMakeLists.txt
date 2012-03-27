project( qtservice )
cmake_minimum_required( VERSION 2.8 )

find_package( Qt4 REQUIRED )
set( QT_USE_QTNETWORK 1 )

include(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set( qtservice_srcs
    qtservice.cpp
    qtservicestarter.cpp
    qtservicesysprivate.cpp
)

set ( qtservice_moc 
    qtservicestarter.h
    qtservicesysprivate.h
)

if ( UNIX ) 
set ( qtservice_srcs ${qtservice_srcs} 
    qtservice_unix.cpp
    qtunixserversocket.cpp
    qtunixsocket.cpp
)
set ( qtservice_moc ${qtservice_moc} 
    qtunixserversocket.h
    qtunixsocket.h
    
)
else()
set ( qtservice_srcs ${qtservice_srcs} 
    qtservice_win.cpp
)
set ( qtservice_moc ${qtservice_moc} )
endif()

set(libraries
    ${QT_LIBRARIES}
)

qt4_wrap_cpp(qtservice_moc_src ${qtservice_moc})

add_library( qtservice STATIC ${qtservice_srcs} ${qtservice_moc_src} )
target_link_libraries( qtservice ${libraries}  )
