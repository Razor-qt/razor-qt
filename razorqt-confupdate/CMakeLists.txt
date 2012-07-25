project(razor-confupdate)
cmake_minimum_required( VERSION 2.6 )

set(PROG_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/razor/${PROJECT_NAME})
add_definitions(-DPROG_SHARE_DIR=\"${PROG_SHARE_DIR}\")
add_definitions(-DLIB_SUFFIX=\"${LIB_SUFFIX}\")

if(NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif(NOT CMAKE_BUILD_TYPE)

add_definitions(-Wall)
find_package(Qt4 REQUIRED)

include( ${QT_USE_FILE} )
include_directories(
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} 
    ${CMAKE_SOURCE_DIR}/libraries/
)

set(${PROJECT_NAME}_HEADERS
    rzupdate.h
    rzwatcher.h
)

set(${PROJECT_NAME}_SRCS
    rzupdate.cpp
    razor-confupdate.cpp
    rzwatcher.cpp
)

set(${PROJECT_NAME}_MOCS
    rzwatcher.h
)

set(LIBRARIES
    qtxdg
)
qt4_wrap_cpp(${PROJECT_NAME}_MOCSCXX ${${PROJECT_NAME}_MOCS})

add_executable ( ${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_MOCSCXX})
target_link_libraries ( ${PROJECT_NAME} ${QT_QTCORE_LIBRARY} ${LIBRARIES} )
add_dependencies(${PROJECT_NAME} razorqt)
INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

if (BUILD_TESTS STREQUAL "Yes")
  add_subdirectory(tests)
endif()

add_subdirectory(updates/razor-0.5)



