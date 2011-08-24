project( razorqt )
cmake_minimum_required( VERSION 2.6 )

add_subdirectory(qtxdg)

set (MAJOR_VERSION 0)
set (MINOR_VERSION 1)
set (PATCH_VERSION 0)

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( X11 REQUIRED )


include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR}
	${X11_INCLUDE_DIR}
	${LIBMAGIC_INCLUDE_DIR}
	.
	razorqt
    librazor-upower
    qxt
)

set(razorqt_HDRS
    razorqt/razoraboutdlg.h
    razorqt/razorsettings.h
    razorqt/xdgautostart.h
    razorqt/xfitman.h
    razorqt/razorplugininfo.h
    razorqt/powermanager.h
    razorqt/addplugindialog/addplugindialog.h
    razorqt/screensaver.h
    razorqt/razorshortcutbutton.h
    razorqt/razorshortcutbutton_p.h
    # upower. Maybe standalone lib?
    librazor-upower/battery.h
    librazor-upower/ucpufreq.h
    librazor-upower/upower.h
)

set(razorqt_SRCS
    razorqt/razoraboutdlg.cpp
    razorqt/xdgautostart.cpp
    razorqt/xfitman.cpp
    razorqt/razorplugininfo.cpp
    razorqt/powermanager.cpp
    razorqt/addplugindialog/addplugindialog.cpp
    razorqt/razorsettings.cpp
    razorqt/screensaver.cpp
    razorqt/razorshortcutbutton.cpp
    # upower. Maybe standalone lib?
    librazor-upower/battery.cpp
    librazor-upower/ucpufreq.cpp
    librazor-upower/upower.cpp
)

set(razorqt_MOCS
    razorqt/razoraboutdlg.h
    razorqt/razorplugininfo.h
    razorqt/powermanager.h
    razorqt/addplugindialog/addplugindialog.h
    razorqt/razorsettings.h
    razorqt/screensaver.h
    razorqt/razorshortcutbutton.h
    razorqt/razorshortcutbutton_p.h
    # upower. Maybe standalone lib?
    librazor-upower/battery.h
    librazor-upower/ucpufreq.h
    librazor-upower/upower.h
)

QT4_WRAP_CPP(MOCS ${razorqt_MOCS})


set ( razorqt_UIS
    razorqt/aboutdlg.ui
    razorqt/addplugindialog/addplugindialog.ui
)

QT4_WRAP_UI(UIS ${razorqt_UIS})


include(RazorLibSuffix)

add_library ( razorqt SHARED ${razorqt_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )

target_link_libraries ( razorqt  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                                 ${QT_QTXML_LIBRARY} ${QT_QTDBUS_LIBRARY}
                                 ${X11_X11_LIB} ${LIBMAGIC_LIBRARY} 
                                 qtxdg
                      )

set_target_properties(razorqt PROPERTIES
  VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
  SOVERSION ${MAJOR_VERSION}
)

install(TARGETS razorqt DESTINATION lib${LIB_SUFFIX})
install(FILES ${razorqt_HDRS} DESTINATION include/razorqt)

# our Qxt library
set ( razorqxt_HDRS
    razorqxt/qxtglobal.h
    razorqxt/qxtglobalshortcut.h
    )
    
set ( razorqxt_SRC
    razorqxt/qxtglobal.cpp
    razorqxt/qxtglobalshortcut.cpp
    razorqxt/qxtglobalshortcut_x11.cpp
    )

QT4_WRAP_CPP(qxt_MOCS ${razorqxt_HDRS})

add_library ( razorqxt SHARED ${razorqxt_SRC} ${qxt_MOCS} )

target_link_libraries ( razorqxt  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                                 ${X11_X11_LIB} )

set_target_properties(razorqxt PROPERTIES
  VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
  SOVERSION ${MAJOR_VERSION}
)

install(TARGETS razorqxt DESTINATION lib${LIB_SUFFIX})
install(FILES ${razorqxt_HDRS} DESTINATION include/razorqxt)


add_subdirectory(razormount)
add_dependencies( razorqt qtxdg razorqxt razormount)


find_package(Doxygen)
if(DOXYGEN_FOUND)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY
    )
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

endif(DOXYGEN_FOUND)

