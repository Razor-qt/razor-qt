project( razor-session )
cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
   set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( X11 REQUIRED )

include(RazorLibSuffix)

include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${X11_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/librazorqt/ . src)

set ( razor-session_HDRS
	)

set ( razor-session_SRCS
    src/main.cpp
    src/razormodman.cpp
    src/wmselectdialog.cpp
	)

set ( razor-session_MOCS
    src/razormodman.h
    src/sessiondbusadaptor.h
    src/wmselectdialog.h
	)

set ( razor-session_UI
    src/wmselectdialog.ui
)

# Translations **********************************
include(RazorTranslate)
razor_translate(QM_FILES)
#************************************************

QT4_WRAP_CPP(MOCS ${razor-session_MOCS})
qt4_wrap_ui(UIS ${razor-session_UI})

add_executable ( razor-session ${razor-session_SRCS} ${UIS} ${RSCS} ${QM_FILES} ${MOCS} )
add_dependencies( razor-session razorqt qtxdg)
target_link_libraries ( razor-session  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTXML_LIBRARY} ${X11_X11_LIB} ${CMAKE_BINARY_DIR}/librazorqt/librazorqt.so qtxdg)
INSTALL(TARGETS razor-session RUNTIME DESTINATION bin)

install(PROGRAMS resources/xdg-open resources/xdg-mime DESTINATION lib${LIB_SUFFIX}/razor-xdg-tools)
add_definitions ( -DPATH_PREPEND=\"${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/razor-xdg-tools\" )

add_subdirectory(config)
