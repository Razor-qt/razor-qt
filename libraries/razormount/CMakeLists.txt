
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
	${X11_INCLUDE_DIR}
	.
)

set(razormount_HDRS
    razormount.h
    rzmountproviders.h
)

set(razormount_SRCS
    razormount.cpp
    rzmountproviders.cpp
    rzmountproviders_udisks2.cpp
)

set(razormount_MOCS
    razormount.h
    rzmountproviders.h
)

QT4_WRAP_CPP(razourmount_MOCS ${razormount_MOCS})

#include(RazorLibSuffix)

add_library ( razormount SHARED ${razormount_SRCS} ${razourmount_MOCS} )

target_link_libraries ( razormount ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                                 ${QT_QTDBUS_LIBRARY}
				 ${UDEV_LIBRARIES} 
                      )

set_target_properties(razormount PROPERTIES
  VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
  SOVERSION ${MAJOR_VERSION}
)

install(TARGETS razormount DESTINATION lib${LIB_SUFFIX})
install(FILES ${razormount_HDRS} DESTINATION include/razormount)

include(create_pkgconfig_file)
create_pkgconfig_file(razormount "Razor-qt library for removable media/devices")
