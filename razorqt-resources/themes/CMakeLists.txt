cmake_minimum_required( VERSION 2.6 )

install(DIRECTORY . DESTINATION share/razor/themes PATTERN "CMake*" EXCLUDE PATTERN "cmake*" EXCLUDE PATTERN "Makefile" EXCLUDE)


