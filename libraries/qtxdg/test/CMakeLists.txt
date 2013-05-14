project(qtxdg_test)
cmake_minimum_required( VERSION 2.6 )


set(${PROJECT_NAME}_SRCS
    qtxdg_test.cpp
)

set(${PROJECT_NAME}_MOCS
    qtxdg_test.h
)


set(LIBRARIES
    qtxdg
)

set(QT_USE_QTTEST true)

add_definitions ( -Wall )

find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )

qt4_wrap_cpp(MOCS ${${PROJECT_NAME}_MOCS})


include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} 
    ${CMAKE_SOURCE_DIR}/libraries/
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( ${PROJECT_NAME} ${QT_LIBRARIES}  ${LIBRARIES} )
add_dependencies(${PROJECT_NAME} razorqt)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
