
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

# our Qxt library
set ( razorqxt_PUBLIC_HDRS
    qxtglobal.h
    qxtglobalshortcut.h
    )

set ( razorqxt_PRIVATE_HDRS
    keymapper_x11.h
    qxtglobalshortcut_p.h
    )

set ( razorqxt_SRC
    qxtglobal.cpp
    qxtglobalshortcut.cpp
    qxtglobalshortcut_x11.cpp
    )

QT4_WRAP_CPP(qxt_MOCS qxtglobalshortcut.h)

add_library( razorqxt SHARED ${razorqxt_SRC} ${qxt_MOCS}  ${QM_FILES} ${razorqxt_PUBLIC_HDRS} ${razorqxt_PRIVATE_HDRS})

target_link_libraries( razorqxt  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                                 ${X11_X11_LIB} )

set_target_properties(razorqxt PROPERTIES
  VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
  SOVERSION ${MAJOR_VERSION}
)

install(TARGETS razorqxt DESTINATION lib${LIB_SUFFIX})
install(FILES ${razorqxt_PUBLIC_HDRS} DESTINATION include/razorqxt)

include(create_pkgconfig_file)
create_pkgconfig_file(razorqxt "Customized Qxt library for Razor-qt project")

