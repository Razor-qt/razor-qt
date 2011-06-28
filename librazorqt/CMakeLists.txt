project( razorqt )
cmake_minimum_required( VERSION 2.6 )

set (MAJOR_VERSION 0)
set (MINOR_VERSION 1)
set (PATCH_VERSION 0)

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( X11 REQUIRED )
find_package ( LibMagic REQUIRED )


include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR}
	${X11_INCLUDE_DIR}
	${LIBMAGIC_INCLUDE_DIR}
	.
	razorqt
    librazor-upower
    )

set ( razorqt_HDRS
    razorqt/razoraboutdlg.h
#   razorqt/razordevman.h
#   razorqt/razorhal.h
    razorqt/razorsettings.h
    razorqt/xdgautostart.h
    razorqt/xdgenv.h
    razorqt/xdgiconthemefile.h
    razorqt/xdgiconthememanager.h
    razorqt/xfitman.h
    razorqt/domhelper.h
    razorqt/xdgdesktopfile.h
    razorqt/xdgmenu/xdgmenu.h
    razorqt/xdgicon.h
    razorqt/xdgaction.h
    razorqt/xdgmenu/xdgmenuwidget.h
    razorqt/razorplugininfo.h
    razorqt/powermanager.h
    razorqt/addplugindialog/addplugindialog.h
    razorqt/razormime.h
    razorqt/screensaver.h
    # upower. Maybe standalone lib?
    librazor-upower/battery.h
    librazor-upower/ucpufreq.h
    librazor-upower/upower.h
	)

set ( razorqt_SRCS
    razorqt/razoraboutdlg.cpp
#   razorqt/razordevman.cpp
#   razorqt/razorhal.cpp
    razorqt/xdgautostart.cpp
    razorqt/xdgdesktopfile.cpp
    razorqt/xdgiconthemefile.cpp
    razorqt/xdgiconthememanager.cpp
#   razorqt/xdgmanager.cpp
    razorqt/xdgmenu/xdgmenu.cpp
    razorqt/xfitman.cpp
    razorqt/domhelper.cpp
    razorqt/xdgmenu/xdgmenuapplinkprocessor.cpp
    razorqt/xdgmenu/xdgmenureader.cpp
    razorqt/xdgmenu/xdgmenurules.cpp
    razorqt/xdgmenu/xdgmenulayoutprocessor.cpp
    razorqt/xdgicon.cpp
    razorqt/xdgaction.cpp
    razorqt/xdgmenu/xdgmenuwidget.cpp
    razorqt/razorplugininfo.cpp
    razorqt/powermanager.cpp
    razorqt/addplugindialog/addplugindialog.cpp
    razorqt/razormime.cpp
    razorqt/razorsettings.cpp
    razorqt/screensaver.cpp
    # upower. Maybe standalone lib?
    librazor-upower/battery.cpp
    librazor-upower/ucpufreq.cpp
    librazor-upower/upower.cpp

	)

set ( razorqt_MOCS
    razorqt/razoraboutdlg.h
#   razorqt/razordevman.h
#   razorqt/razorhal.h
#   razorqt/xdgmanager.h
    razorqt/xdgmenu/xdgmenu.h
    razorqt/xdgmenu/xdgmenu_p.h
    razorqt/xdgdesktopfile.h
    razorqt/xdgmenu/xdgmenuapplinkprocessor.h
    razorqt/xdgmenu/xdgmenureader.h
    razorqt/xdgmenu/xdgmenurules.h
    razorqt/xdgaction.h
    razorqt/xdgmenu/xdgmenuwidget.h
    razorqt/razorplugininfo.h
    razorqt/powermanager.h
    razorqt/addplugindialog/addplugindialog.h
    razorqt/razorsettings.h
    razorqt/screensaver.h
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
                                 ${X11_X11_LIB} ${LIBMAGIC_LIBRARY} )

set_target_properties(razorqt PROPERTIES
  VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
  SOVERSION ${MAJOR_VERSION}
)


SET(DOXYGEN_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/razorqt
    )

#find_package(Doxygen)
#if(DOXYGEN_FOUND)
#
#    STRING(REGEX REPLACE ";" " " DOXYGEN_INPUT_LIST "${DOXYGEN_SOURCE_DIR}")
#
#    configure_file(
#        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
#        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY
#    )
#    add_custom_target(doc
#        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        COMMENT "Generating API documentation with Doxygen" VERBATIM
#    )
#endif(DOXYGEN_FOUND)


install(TARGETS razorqt DESTINATION lib${LIB_SUFFIX})
install(FILES ${razorqt_HDRS} DESTINATION include/razorqt)

