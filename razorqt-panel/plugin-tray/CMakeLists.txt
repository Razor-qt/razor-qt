set(PLUGIN "tray")

set(HEADERS
  razortrayplugin.h
  razortray.h
  trayicon.h
)

set(SOURCES
  razortrayplugin.cpp
  razortray.cpp
  trayicon.cpp
)

set(MOCS
  razortrayplugin.h
  razortray.h
  trayicon.h
)

include( FindPkgConfig )
include( CheckLibraryExists )

find_package(X11 REQUIRED)
pkg_check_modules( XCOMPOSITE REQUIRED xcomposite)
pkg_check_modules( XDAMAGE REQUIRED xdamage )
pkg_check_modules( XRENDER REQUIRED xrender )
set(LIBRARIES
    ${X11_LIBRARIES}
    ${XCOMPOSITE_LIBRARIES}
    ${XDAMAGE_LIBRARIES}
    ${XRENDER_LIBRARIES}
)

#*******************************************
include ("../BuildPlugin.cmake")



BUILD_RAZOR_PLUGIN(${PLUGIN})
#target_link_libraries( tint2)
