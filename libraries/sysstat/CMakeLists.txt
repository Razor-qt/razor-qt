project(libsysstat)

set (MAJOR_VERSION 0)
set (MINOR_VERSION 1)
set (PATCH_VERSION 0)
add_definitions(-DMAJOR_VERSION=${MAJOR_VERSION})
add_definitions(-DMINOR_VERSION=${MINOR_VERSION})
add_definitions(-DPATCH_VERSION=${PATCH_VERSION})
add_definitions(-DSYSSTAT_LIBRARY)

add_definitions(-Wall)


set(SYSSTAT_PUBLIC_HDRS
    version.h
    sysstat_global.h
    basestat.h
    cpustat.h
    memstat.h
    netstat.h
)

set(SYSSTAT_PRIVATE_HDRS
    version_p.h
    basestat_p.h
    cpustat_p.h
    memstat_p.h
    netstat_p.h
)

set(SYSSTAT_SRCS
    version.cpp
    basestat.cpp
    cpustat.cpp
    memstat.cpp
    netstat.cpp
)

set(SYSSTAT_MOCS
    basestat.h
    cpustat.h
    memstat.h
    netstat.h
    basestat_p.h
    cpustat_p.h
    memstat_p.h
    netstat_p.h
)


# Translations **********************************
file(GLOB SYSSTAT_TS_FILES
    translations/*.ts
)
#************************************************


#**********************************************************


QT4_WRAP_CPP(SYSSTAT_CXX ${SYSSTAT_MOCS})
qt4_add_translation(SYSSTAT_QM_FILES ${SYSSTAT_TS_FILES})

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)


set(APP_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/sysstat)

include_directories( .. )
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
