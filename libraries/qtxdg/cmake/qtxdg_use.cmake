# - Find the Razor-qt include and library dirs and define a some macros
#


find_package(Qt4 REQUIRED QUIET)
include(${QT_USE_FILE})

include_directories(${QTXDG_INCLUDE_DIRS})

