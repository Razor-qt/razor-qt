project( razorqt )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( X11 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR} 
	${X11_INCLUDE_DIR}
	.
	razorqt)
set ( razorqt_HDRS
	razorqt/defs.h
	razorqt/readsettings.h
	razorqt/xdgautostart.h
	razorqt/xdgdesktopfile.h
	razorqt/xdgiconthemefile.h
	razorqt/xdgiconthememanager.h
	razorqt/xdgmenucross.h
	razorqt/xfitman.h
	)

set ( razorqt_SRCS
	razorqt/readsettings.cpp
	razorqt/xdgautostart.cpp
	razorqt/xdgdesktopfile.cpp
	razorqt/xdgenv.cpp
	razorqt/xdgiconthemefile.cpp
	razorqt/xdgiconthememanager.cpp
	razorqt/xdgmanager.cpp
	razorqt/xdgmenu.cpp
	razorqt/xdgmenucross.cpp
	razorqt/xdgreader.cpp
	razorqt/xfitman.cpp
	)

set ( razorqt_MOCS
	razorqt/xdgenv.h
	razorqt/xdgmanager.h
	razorqt/xdgmenu.h
	razorqt/xdgreader.h
	)
QT4_WRAP_CPP(MOCS ${razorqt_MOCS})

add_library ( razorqt SHARED ${razorqt_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( razorqt  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTDBUS_LIBRARY}  ${X11_X11_LIB})
message(STATUS ${CMAKE_INSTALL_PREFIX})



install(TARGETS razorqt DESTINATION lib)                                                                                                                                                                                    
install(FILES ${razorqt_HDRS} DESTINATION include/razorqt)  
