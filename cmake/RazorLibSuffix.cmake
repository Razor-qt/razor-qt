# some system (rpm builds) setup LIB_SUFFIX for cmake. If there is no set, try to get it from system
IF (NOT DEFINED LIB_SUFFIX AND NOT DEFINED LIB_SUFFIX_ALREADY_SET)
    MESSAGE(STATUS "*********************************************************************")
    MESSAGE(STATUS "LIB_SUFFIX variable is not defined. It will be autodetected now")
    MESSAGE(STATUS "You can set it manually with -DLIB_SUFFIX=<value> (64 for example)")

    # All 32bit system have empty lib suffix
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        # If there is lib64 dir, set suffix to 64
        if(IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib64)
            set(LIB_SUFFIX 64)
        elseif(IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)
            set(LIB_SUFFIX "")
        else()
            message(WARNING "LIB_SUFFIX cannot be autodetected. No ${CMAKE_INSTALL_PREFIX}/lib neither ${CMAKE_INSTALL_PREFIX}/lib64 found.")
            set(LIB_SUFFIX "")
        endif()
    else()
        set(LIB_SUFFIX "")
    endif()
    
    set(LIB_SUFFIX_ALREADY_SET 1)

    message(STATUS "LIB_SUFFIX autodetected as '${LIB_SUFFIX}', libraries will be installed into ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
    MESSAGE(STATUS "*********************************************************************")
else ()
    message(STATUS "LIB_SUFFIX already set: ${LIB_SUFFIX}")
ENDIF ()
