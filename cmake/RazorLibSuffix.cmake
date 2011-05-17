# some system (rpm builds) setup LIB_SUFFIX for cmake. If there is no set, try to get it from system
IF (NOT DEFINED LIB_SUFFIX)
	MESSAGE(STATUS "")
	MESSAGE(STATUS "LIB_SUFFIX variable is not defined. It will be autodetected now.")
	MESSAGE(STATUS "You can set it manually with -DLIB_SUFFIX=<value> (64 for example)")
	IF (NOT APPLE)
		# check 64 bit
		IF (CMAKE_SIZEOF_VOID_P EQUAL 4)
			SET (LIB_SUFFIX "")
		ELSE (CMAKE_SIZEOF_VOID_P EQUAL 4)
			SET (LIB_SUFFIX "64")
			MESSAGE(STATUS " LIB_SUFFIX is set to '${LIB_SUFFIX}'")
		ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 4)
	ELSE (NOT APPLE)
		SET (LIB_SUFFIX "")
	ENDIF (NOT APPLE)
	# BSD does not use lib64 for 64bit libs
	IF (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
		message(STATUS "LIB_SUFFIX is disabled for *BSD: ${CMAKE_SYSTEM_NAME}")
		SET (LIB_SUFFIX "")
	ENDIF()
	#
	MESSAGE(STATUS "")
ENDIF (NOT DEFINED LIB_SUFFIX)
