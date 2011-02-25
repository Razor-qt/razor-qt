project( razor-su )
cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( X11 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    ${X11_INCLUDE_DIR}
	src)

set ( razor-su_HDRS
    src/sudialog.h
    src/suprocess.h
	)

set ( razor-su_SRCS
    src/main.cpp
    src/suprocess.cpp
    src/sudialog.cpp
	)

QT4_WRAP_CPP(MOCS ${razor-su_HDRS})
QT4_WRAP_UI(UIS src/sudialog.ui)

SET (exec razor-su)

add_executable ( ${exec} ${razor-su_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( ${exec}  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${X11_X11_LIB})
INSTALL(TARGETS ${exec} RUNTIME DESTINATION bin)
