project( razor-openssh-askpass )
cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
    ${CMAKE_SOURCE_DIR}/libraries
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	src
)

set ( razor-openssh-askpass_HDRS
	src/mainwindow.h
	)

set ( razor-openssh-askpass_SRCS
	src/main.cpp
	src/mainwindow.cpp
	)



QT4_WRAP_CPP(MOCS ${razor-openssh-askpass_HDRS})
QT4_WRAP_UI(UIS src/mainwindow.ui)


add_executable ( razor-openssh-askpass ${razor-openssh-askpass_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
add_dependencies( razor-openssh-askpass razorqt )
target_link_libraries(razor-openssh-askpass ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} razorqt)
INSTALL(TARGETS razor-openssh-askpass RUNTIME DESTINATION bin)
