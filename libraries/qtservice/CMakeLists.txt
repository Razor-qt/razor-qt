project( qtservice )

find_package( Qt4 REQUIRED )
set( QT_USE_QTNETWORK 1 )

include(RazorLibSuffix)
include(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set( qtservice_srcs
    qtservice.cpp
    qtservicestarter.cpp
    qtservicesysprivate.cpp
)

set ( qtservice_pub_headers
    QtServiceBase
    QtServiceController
    qtservice.h
    qtservicestarter.h
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
  set ( qtservice_pub_headers ${qtservice_pub_headers}
      qtunixsocket.h
      qtunixserversocket.h
  )
  
else()
  set ( qtservice_srcs ${qtservice_srcs} 
      qtservice_win.cpp
  )
endif()

set(libraries
    ${QT_LIBRARIES}
)

qt4_wrap_cpp(qtservice_moc_src ${qtservice_moc})

add_library( qtservice SHARED ${qtservice_srcs} ${qtservice_moc_src} ${qtservice_pub_headers})
target_link_libraries( qtservice ${libraries}  )

install(TARGETS qtservice DESTINATION lib${LIB_SUFFIX})
install(FILES ${qtservice_pub_headers} DESTINATION include/qtxdg)