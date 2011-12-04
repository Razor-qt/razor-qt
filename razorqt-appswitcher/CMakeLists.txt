project( razor-desktop )
cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR}
    ${X11_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/
	src
)

set ( razor-appswitcher_HDRS
	src/appswitcher.h
	src/application.h
	)

set ( razor-appswitcher_SRCS
	src/main.cpp
	src/appswitcher.cpp
	src/application.cpp
	)

set(LIBRARIES
    razorqt
    razorqxt
)

QT4_WRAP_CPP(MOCS ${razor-appswitcher_HDRS})
QT4_WRAP_UI(UIS src/appswitcher.ui)

SET (exec razor-appswitcher)

add_executable ( ${exec} ${razor-appswitcher_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
add_dependencies( ${exec} razorqt )
target_link_libraries(${exec}   ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                                ${LIBRARIES}
                    )
INSTALL(TARGETS ${exec} RUNTIME DESTINATION bin)
