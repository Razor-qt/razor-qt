project(sesion_modules)
cmake_minimum_required( VERSION 2.6 )

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )

include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} 
    ${CMAKE_SOURCE_DIR}/libraries/
)

set(${PROJECT_NAME}_SRCS
    main.cpp
)

set(LIBRARIES
    qtxdg
)

add_executable ( ${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})
target_link_libraries ( ${PROJECT_NAME} ${QT_QTCORE_LIBRARY} ${LIBRARIES} )
add_dependencies(${PROJECT_NAME} razorqt)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION lib${LIB_SUFFIX}/razor-confupdate_bin)

install(FILES desktop-041-050.py DESTINATION share/razor/razor-confupdate)

install(FILES razor-0.5.upd DESTINATION share/razor/razor-confupdate) 
