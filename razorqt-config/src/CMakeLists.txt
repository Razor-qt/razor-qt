
include_directories (
	${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} qtxdg)

set ( razor-config_HDRS
	)

set ( razor-config_SRCS
	main.cpp
	mainwindow.cpp
	)

set ( razor-config_MOCS
    mainwindow.h
	)

QT4_WRAP_CPP(MOCS ${razor-config_MOCS})
QT4_WRAP_UI(UIS mainwindow.ui)

add_executable ( razor-config ${razor-config_SRCS} ${UIS} ${MOCS} )
add_dependencies( razor-config razorqt qtxdg)
target_link_libraries ( razor-config  
                ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY}
                qtxdg razorqt)

INSTALL(TARGETS razor-config RUNTIME DESTINATION bin)
install(FILES razor-config.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
