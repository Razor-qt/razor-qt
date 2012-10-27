project(libsysstat)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set (MAJOR_VERSION 0)
set (MINOR_VERSION 1)
set (PATCH_VERSION 0)
add_definitions(-DMAJOR_VERSION=${MAJOR_VERSION})
add_definitions(-DMINOR_VERSION=${MINOR_VERSION})
add_definitions(-DPATCH_VERSION=${PATCH_VERSION})
add_definitions(-DSYSSTAT_LIBRARY)

add_definitions(-Wall)
find_package(Qt4 REQUIRED)


set(SYSSTAT_PUBLIC_HDRS
    version.hpp
    sysstat_global.hpp
    basestat.hpp
    cpustat.hpp
    memstat.hpp
    netstat.hpp
)

set(SYSSTAT_PRIVATE_HDRS
    version_p.hpp
    basestat_p.hpp
    cpustat_p.hpp
    memstat_p.hpp
    netstat_p.hpp
)

set(SYSSTAT_SRCS
    version.cpp
    basestat.cpp
    cpustat.cpp
    memstat.cpp
    netstat.cpp
)

set(SYSSTAT_MOCS
    basestat.hpp
    cpustat.hpp
    memstat.hpp
    netstat.hpp
    basestat_p.hpp
    cpustat_p.hpp
    memstat_p.hpp
    netstat_p.hpp
)


# Translations **********************************
file(GLOB SYSSTAT_TS_FILES
    translations/*.ts
)
#************************************************


#**********************************************************
cmake_minimum_required( VERSION 2.6 )

include(RazorLibSuffix)
include(${QT_USE_FILE})

QT4_WRAP_CPP(SYSSTAT_CXX ${SYSSTAT_MOCS})
qt4_add_translation(SYSSTAT_QM_FILES ${SYSSTAT_TS_FILES})

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)


set(APP_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/sysstat)
add_definitions(-DTRANSLATIONS_DIR=\"${APP_SHARE_DIR}\")

add_library(sysstat SHARED ${SYSSTAT_PUBLIC_HDRS} ${SYSSTAT_PRIVATE_HDRS} ${SYSSTAT_SRCS} ${SYSSTAT_CXX} ${SYSSTAT_QM_FILES})
target_link_libraries(sysstat  ${QT_LIBRARIES})

set_target_properties(sysstat PROPERTIES
  VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
  SOVERSION ${MAJOR_VERSION}
)

install(TARGETS sysstat DESTINATION lib${LIB_SUFFIX})
install(FILES ${SYSSTAT_PUBLIC_HDRS} DESTINATION include/sysstat)
install(FILES ${SYSSTAT_QM_FILES}    DESTINATION ${APP_SHARE_DIR})

include(create_pkgconfig_file)
create_pkgconfig_file(sysstat "SysStat, a Qt-based interface to system statistics")
