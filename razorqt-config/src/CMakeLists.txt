# build static helper class first
add_subdirectory(qcategorizedview)

include_directories (
        ${CMAKE_CURRENT_BINARY_DIR}
        ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/libraries
        ${CMAKE_CURRENT_SOURCE_DIR}/qcategorizedview
        )

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
# Translations **********************************
include(RazorTranslate)
razor_translate(QM_FILES)
razor_translate_desktop(DESKTOP_FILES razor-config.desktop.in)
#************************************************

add_executable ( razor-config ${razor-config_SRCS} ${UIS} ${MOCS} ${DESKTOP_FILES})
add_dependencies( razor-config razorqt qtxdg)
target_link_libraries ( razor-config
                ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY}
                qtxdg razorqt)
# helper static lib
link_directories(./qcategorizedview)
target_link_libraries( razor-config qcategorizedview)

INSTALL(TARGETS razor-config RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
