cmake_minimum_required( VERSION 2.6 )

install(DIRECTORY config/ DESTINATION share/razor)
install(DIRECTORY graphics DESTINATION share/razor/)

add_subdirectory(autostart )
add_subdirectory( sys )
add_subdirectory( themes )
add_subdirectory( application-menu )
