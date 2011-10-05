if (NOT UDEV_STABLE)
    message(WARNING "libudev is not in the stable API version. razormount will not be compiled")
else (NOT UDEV_STABLE)

include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR}
	${X11_INCLUDE_DIR}
	.
)

set(razormount_HDRS
    # udisks. Maybe standalone lib too?
    diskmonitor.h
    mount.h
    storageitem.h
    storagemanager.h
)

set(razormount_SRCS
    # udisks. Maybe standalone lib too?
    diskmonitor.cpp
    mount.cpp
    storageitem.cpp
    storagemanager.cpp
)

set(razormount_MOCS
    diskmonitor.h
    storageitem.h
    storagemanager.h
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

endif (NOT UDEV_STABLE)

