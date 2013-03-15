project(global_action_native_client)

cmake_minimum_required(VERSION 2.8.3)

enable_language(CXX)
add_definitions(-Wall)

add_definitions(-DNATIVE_CLIENT_LIBRARY)

set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 1)
set(FULL_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR})
message(STATUS "")
message(STATUS "Version: ${FULL_VERSION}")
message(STATUS "")
add_definitions(-D${PROJECT_NAME}_VERSION_MAJOR="${${PROJECT_NAME}_VERSION_MAJOR}")
add_definitions(-D${PROJECT_NAME}_VERSION_MINOR="${${PROJECT_NAME}_VERSION_MINOR}")
add_definitions(-DFULL_VERSION="${FULL_VERSION}")


include_directories(
	"${PROJECT_SOURCE_DIR}"
	"${CMAKE_CURRENT_BINARY_DIR}"
)


if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE MATCHES [Dd]ebug)
	message(STATUS "Debug build")
	add_definitions(-DDEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else()
	message(STATUS "Release build")
	add_definitions(-DNDEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()


if(CMAKE_PREFERRED_QT EQUAL 4)
	find_package(Qt4 4.4.0 COMPONENTS QtCore QtDBus)
elseif(CMAKE_PREFERRED_QT EQUAL 5)
	find_package(Qt5Core)
	find_package(Qt5DBus)
else()
	find_package(Qt5Core)
	find_package(Qt5DBus)

	if(NOT Qt5Core_VERSION_MAJOR)
		find_package(Qt4 4.4.0 COMPONENTS QtCore QtDBus)
	endif()
endif()

if(Qt5Core_VERSION_MAJOR)
	set(QT_VERSION_MAJOR ${Qt5Core_VERSION_MAJOR})
	set(QT_VERSION_MINOR ${Qt5Core_VERSION_MINOR})
	set(QT_VERSION_PATCH ${Qt5Core_VERSION_PATCH})
	set(QTVERSION ${Qt5Core_VERSION_STRING})
endif()

message(STATUS "Using Qt version ${QTVERSION}")

if(QT_VERSION_MAJOR EQUAL 4)
	include(${QT_USE_FILE})
else()
	if(CMAKE_CXX_COMPILER MATCHES "^(.*/)?clang\\+\\+$")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
	endif()
endif()



set(SHARED_EXPORT Q_DECL_EXPORT)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/native_client.hpp.in"
	"${CMAKE_CURRENT_BINARY_DIR}/native_client_export.hpp"
	IMMEDIATE @ONLY)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/global_action.hpp.in"
	"${CMAKE_CURRENT_BINARY_DIR}/global_action_export.hpp"
	IMMEDIATE @ONLY)
set(SHARED_EXPORT Q_DECL_IMPORT)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/native_client.hpp.in"
	"${CMAKE_CURRENT_BINARY_DIR}/native_client.hpp"
	IMMEDIATE @ONLY)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/global_action.hpp.in"
	"${CMAKE_CURRENT_BINARY_DIR}/global_action.hpp"
	IMMEDIATE @ONLY)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.hpp.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hpp"
	IMMEDIATE @ONLY)
unset(SHARED_EXPORT)



set(${PROJECT_NAME}_SOURCES
	native_client.cpp
	global_action.cpp
)

set(${PROJECT_NAME}_MAIN_HEADER
	"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hpp"
)

set(${PROJECT_NAME}_PUBLIC_CPP_HEADERS
	"${CMAKE_CURRENT_BINARY_DIR}/native_client.hpp" # it's not used, just generated, so it isn't a QT header
	"${CMAKE_CURRENT_BINARY_DIR}/global_action.hpp" # it's not used, just generated, so it isn't a QT header
)

set(${PROJECT_NAME}_PRIVATE_CPP_HEADERS
)

set(${PROJECT_NAME}_PUBLIC_QT_HEADERS
)

set(${PROJECT_NAME}_PRIVATE_QT_HEADERS
	"${CMAKE_CURRENT_BINARY_DIR}/native_client_export.hpp"
	"${CMAKE_CURRENT_BINARY_DIR}/global_action_export.hpp"
	native_client_p.hpp
	global_action_p.hpp
)

set(${PROJECT_NAME}_FORMS
)

set(${PROJECT_NAME}_RESOURCES
)

set(${PROJECT_NAME}_TRANSLATIONS
)

set(${PROJECT_NAME}_DBUS_ADAPTERS
	../daemon/org.razorqt.global_action.client.xml
)

set_source_files_properties(../daemon/org.razorqt.global_action.client.xml PROPERTIES PARENT global_action_p.hpp)
set_source_files_properties(../daemon/org.razorqt.global_action.client.xml PROPERTIES PARENT_CLASSNAME GlobalActionImpl)
set_source_files_properties(../daemon/org.razorqt.global_action.client.xml PROPERTIES BASENAME org.razorqt.global_action.client)
set_source_files_properties(../daemon/org.razorqt.global_action.client.xml PROPERTIES CLASSNAME OrgRazorqtGlobalActionClientAdaptor)

set(${PROJECT_NAME}_DBUS_PROXYS
	../daemon/org.razorqt.global_action.native.xml
)

set_source_files_properties(../daemon/org.razorqt.global_action.native.xml PROPERTIES BASENAME org.razorqt.global_action.native)



if(${QT_VERSION_MAJOR} EQUAL 4)
	foreach(DBUS_ADAPTOR ${${PROJECT_NAME}_DBUS_ADAPTERS})
		get_source_file_property(DBUS_ADAPTOR_INCLUDE ${DBUS_ADAPTOR} PARENT)
		get_source_file_property(DBUS_ADAPTOR_PARENT_CLASSNAME ${DBUS_ADAPTOR} PARENT_CLASSNAME)
		get_source_file_property(DBUS_ADAPTOR_BASENAME ${DBUS_ADAPTOR} BASENAME)
		get_source_file_property(DBUS_ADAPTOR_CLASSNAME ${DBUS_ADAPTOR} CLASSNAME)
		if(DBUS_ADAPTOR_BASENAME)
			if(DBUS_ADAPTOR_CLASSNAME)
				qt4_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_INCLUDE} ${DBUS_ADAPTOR_PARENT_CLASSNAME} ${DBUS_ADAPTOR_BASENAME} ${DBUS_ADAPTOR_CLASSNAME})
			else()
				qt4_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_INCLUDE} ${DBUS_ADAPTOR_PARENT_CLASSNAME} ${DBUS_ADAPTOR_BASENAME})
			endif()
		else()
			qt4_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_INCLUDE} ${DBUS_ADAPTOR_PARENT_CLASSNAME})
		endif()
	endforeach()

	foreach(DBUS_INTERFACE ${${PROJECT_NAME}_DBUS_PROXYS})
		get_source_file_property(DBUS_INTERFACE_BASENAME ${DBUS_INTERFACE} BASENAME)
		qt4_add_dbus_interface(${PROJECT_NAME}_DBUS_INTERFACE_FILES ${DBUS_INTERFACE} ${DBUS_INTERFACE_BASENAME})
	endforeach()
elseif(${QT_VERSION_MAJOR} EQUAL 5)
	foreach(DBUS_ADAPTOR ${${PROJECT_NAME}_DBUS_ADAPTERS})
		get_source_file_property(DBUS_ADAPTOR_INCLUDE ${DBUS_ADAPTOR} PARENT)
		get_source_file_property(DBUS_ADAPTOR_PARENT_CLASSNAME ${DBUS_ADAPTOR} PARENT_CLASSNAME)
		get_source_file_property(DBUS_ADAPTOR_BASENAME ${DBUS_ADAPTOR} BASENAME)
		get_source_file_property(DBUS_ADAPTOR_CLASSNAME ${DBUS_ADAPTOR} CLASSNAME)
		if(DBUS_ADAPTOR_BASENAME)
			if(DBUS_ADAPTOR_CLASSNAME)
				qt5_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_INCLUDE} ${DBUS_ADAPTOR_PARENT_CLASSNAME} ${DBUS_ADAPTOR_BASENAME} ${DBUS_ADAPTOR_CLASSNAME})
			else()
				qt5_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_INCLUDE} ${DBUS_ADAPTOR_PARENT_CLASSNAME} ${DBUS_ADAPTOR_BASENAME})
			endif()
		else()
			qt5_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_INCLUDE} ${DBUS_ADAPTOR_PARENT_CLASSNAME})
		endif()
	endforeach()

	foreach(DBUS_INTERFACE ${${PROJECT_NAME}_DBUS_PROXYS})
		get_source_file_property(DBUS_INTERFACE_BASENAME ${DBUS_INTERFACE} BASENAME)
		qt5_add_dbus_interface(${PROJECT_NAME}_DBUS_INTERFACE_FILES ${DBUS_INTERFACE} ${DBUS_INTERFACE_BASENAME})
	endforeach()
endif()

set(${PROJECT_NAME}_PUBLIC_HEADERS
	${${PROJECT_NAME}_PUBLIC_CPP_HEADERS}
	${${PROJECT_NAME}_PUBLIC_QT_HEADERS}
)

set(${PROJECT_NAME}_PRIVATE_HEADERS
	${${PROJECT_NAME}_PRIVATE_CPP_HEADERS}
	${${PROJECT_NAME}_PRIVATE_QT_HEADERS}
)

set(${PROJECT_NAME}_CPP_HEADERS
	${${PROJECT_NAME}_PUBLIC_CPP_HEADERS}
	${${PROJECT_NAME}_PRIVATE_CPP_HEADERS}
)

set(${PROJECT_NAME}_QT_HEADERS
	${${PROJECT_NAME}_PUBLIC_QT_HEADERS}
	${${PROJECT_NAME}_PRIVATE_QT_HEADERS}
)

set(${PROJECT_NAME}_HEADERS
	${${PROJECT_NAME}_PUBLIC_HEADERS}
	${${PROJECT_NAME}_PRIVATE_HEADERS}
)

set(${PROJECT_NAME}_TRANSLATABLE
	${${PROJECT_NAME}_SOURCES}
	${${PROJECT_NAME}_HEADERS}
	${${PROJECT_NAME}_FORMS}
)

if(${QT_VERSION_MAJOR} EQUAL 4)
	qt4_wrap_cpp(${PROJECT_NAME}_MOC_FILES ${${PROJECT_NAME}_QT_HEADERS})
	qt4_add_resources(${PROJECT_NAME}_RESOURCE_FILES ${${PROJECT_NAME}_RESOURCES})
	if(COMMAND qt4_wrap_ui)
		qt4_wrap_ui(${PROJECT_NAME}_FORM_FILES ${${PROJECT_NAME}_FORMS})
	endif()
	if(COMMAND qt4_create_translation)
		qt4_create_translation(${${PROJECT_NAME}_QM_FILES} ${${PROJECT_NAME}_TRANSLATABLE} ${${PROJECT_NAME}_TRANSLATIONS})
	endif()
elseif(${QT_VERSION_MAJOR} EQUAL 5)
	qt5_wrap_cpp(${PROJECT_NAME}_MOC_FILES ${${PROJECT_NAME}_QT_HEADERS})
	qt5_add_resources(${PROJECT_NAME}_RESOURCE_FILES ${${PROJECT_NAME}_RESOURCES})
	if(COMMAND qt5_wrap_ui)
		qt5_wrap_ui(${PROJECT_NAME}_FORM_FILES ${${PROJECT_NAME}_FORMS})
	endif()
	if(COMMAND qt5_create_translation)
		qt5_create_translation(${${PROJECT_NAME}_QM_FILES} ${${PROJECT_NAME}_TRANSLATABLE} ${${PROJECT_NAME}_TRANSLATIONS})
	endif()
endif()

set(${PROJECT_NAME}_GENERATED_FILES
	${${PROJECT_NAME}_MOC_FILES}
	${${PROJECT_NAME}_FORM_FILES}
	${${PROJECT_NAME}_RESOURCE_FILES}
	${${PROJECT_NAME}_QM_FILES}
	${${PROJECT_NAME}_DBUS_INTERFACE_FILES}
	${${PROJECT_NAME}_DBUS_ADAPTOR_FILES}
)

set(${PROJECT_NAME}_ALL_FILES
	${${PROJECT_NAME}_SOURCES}
	${${PROJECT_NAME}_HEADERS}
	${${PROJECT_NAME}_GENERATED_FILES}
)


add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${FULL_VERSION})

if(${QT_VERSION_MAJOR} EQUAL 4)
	target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})
elseif(${QT_VERSION_MAJOR} EQUAL 5)
	qt5_use_modules(${PROJECT_NAME} Core DBus)
endif()

configure_file(
	"${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.pc.in"
	"${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc"
	@ONLY
)

install(TARGETS ${PROJECT_NAME} DESTINATION lib COMPONENT runtime)
install(FILES ${${PROJECT_NAME}_MAIN_HEADER} DESTINATION include/ COMPONENT development)
install(FILES ${${PROJECT_NAME}_PUBLIC_HEADERS} DESTINATION include/${PROJECT_NAME} COMPONENT development)
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION lib/pkgconfig COMPONENT development)



# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)

add_custom_target(uninstall
	COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
