project(razor-confupdate_test)
cmake_minimum_required( VERSION 2.6 )


set(${PROJECT_NAME}_SRCS
    test_rzupdate.cpp
)

set(${PROJECT_NAME}_MOCS
    test_rzupdate.h
)


set(LIBRARIES
    qtxdg
)

set(QT_USE_QTTEST true)

add_definitions ( -Wall )

find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )

message(STATUS "IN: ${${PROJECT_NAME}_MOCS}")
qt4_wrap_cpp(MOCS ${${PROJECT_NAME}_MOCS})
message(STATUS "OUT: ${MOCS}")


include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} 
    ${CMAKE_SOURCE_DIR}/libraries/
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( ${PROJECT_NAME} ${QT_LIBRARIES}  ${LIBRARIES} )
add_dependencies(${PROJECT_NAME} razorqt)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

