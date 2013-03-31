project(global_action_manager)

cmake_minimum_required(VERSION 2.8.3)
add_definitions(-Wall)


set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 1)
set(FULL_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR})
message(STATUS "")
message(STATUS "Version: ${FULL_VERSION}")
message(STATUS "")


if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE MATCHES [Dd]ebug)
	message(STATUS "Debug build")
	add_definitions(-DDEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
else()
	message(STATUS "Release build")
	add_definitions(-DNDEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()


find_package(Qt4 4.4.0 COMPONENTS QtCore QtGui QtDBus)

if(NOT QT4_FOUND)
	find_package(Qt5Core)
	find_package(Qt5Widgets)
	find_package(Qt5DBus)
	find_package(Qt5LinguistTools)
endif()

message(STATUS "Using Qt version ${QTVERSION}")

if(${QT_VERSION_MAJOR} EQUAL 4)
	include(${QT_USE_FILE})
endif()


include_directories(
	"${PROJECT_SOURCE_DIR}"
	"${CMAKE_CURRENT_BINARY_DIR}"
)


set(${PROJECT_NAME}_SOURCES
	main.cpp
	main_window.cpp
	action_model.cpp
)

set(${PROJECT_NAME}_FORMS
	main_window.ui
)

set(${PROJECT_NAME}_RESOURCES
)

set(${PROJECT_NAME}_TRANSLATIONS
)

set(${PROJECT_NAME}_CPP_HEADERS
)

set(${PROJECT_NAME}_QT_HEADERS
	main_window.hpp
	action_model.hpp
)

set(${PROJECT_NAME}_DBUS_ADAPTORS
)

set(${PROJECT_NAME}_DBUS_INTERFACES
	../daemon/org.razorqt.global_action.daemon.xml
)
set_source_files_properties(../daemon/org.razorqt.global_action.daemon.xml PROPERTIES CLASSNAME OrgRazorqtGlobalActionDaemonInterface)
set_source_files_properties(../daemon/org.razorqt.global_action.daemon.xml PROPERTIES BASENAME org.razorqt.global_action.daemon)

if(${QT_VERSION_MAJOR} EQUAL 4)
	set(${PROJECT_NAME}_CPP_HEADERS
		${${PROJECT_NAME}_CPP_HEADERS}
		meta_types.hpp
	)
	set_source_files_properties(../daemon/org.razorqt.global_action.daemon.xml PROPERTIES INCLUDE meta_types.hpp)
endif()



if(${QT_VERSION_MAJOR} EQUAL 4)
	foreach(DBUS_ADAPTOR ${${PROJECT_NAME}_DBUS_ADAPTORS})
		get_source_file_property(DBUS_ADAPTOR_PARENT ${DBUS_ADAPTOR} PARENT)
		get_source_file_property(DBUS_ADAPTOR_PARENT_CLASSNAME ${DBUS_ADAPTOR} PARENT_CLASSNAME)
		get_source_file_property(DBUS_ADAPTOR_BASENAME ${DBUS_ADAPTOR} BASENAME)
		get_source_file_property(DBUS_ADAPTOR_CLASSNAME ${DBUS_ADAPTOR} CLASSNAME)
		if(DBUS_ADAPTOR_BASENAME)
			if(DBUS_ADAPTOR_CLASSNAME)
				qt4_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_PARENT} ${DBUS_ADAPTOR_PARENT_CLASSNAME} ${DBUS_ADAPTOR_BASENAME} ${DBUS_ADAPTOR_CLASSNAME})
			else()
				qt4_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_PARENT} ${DBUS_ADAPTOR_PARENT_CLASSNAME} ${DBUS_ADAPTOR_BASENAME})
			endif()
		else()
			qt4_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_PARENT} ${DBUS_ADAPTOR_PARENT_CLASSNAME})
		endif()
	endforeach()

	foreach(DBUS_INTERFACE ${${PROJECT_NAME}_DBUS_INTERFACES})
		get_source_file_property(DBUS_INTERFACE_BASENAME ${DBUS_INTERFACE} BASENAME)
		qt4_add_dbus_interface(${PROJECT_NAME}_DBUS_INTERFACE_FILES ${DBUS_INTERFACE} ${DBUS_INTERFACE_BASENAME})
	endforeach()
elseif(${QT_VERSION_MAJOR} EQUAL 5)
	foreach(DBUS_ADAPTOR ${${PROJECT_NAME}_DBUS_ADAPTORS})
		get_source_file_property(DBUS_ADAPTOR_PARENT ${DBUS_ADAPTOR} PARENT)
		get_source_file_property(DBUS_ADAPTOR_PARENT_CLASSNAME ${DBUS_ADAPTOR} PARENT_CLASSNAME)
		get_source_file_property(DBUS_ADAPTOR_BASENAME ${DBUS_ADAPTOR} BASENAME)
		get_source_file_property(DBUS_ADAPTOR_CLASSNAME ${DBUS_ADAPTOR} CLASSNAME)
		if(DBUS_ADAPTOR_BASENAME)
			if(DBUS_ADAPTOR_CLASSNAME)
				qt5_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_PARENT} ${DBUS_ADAPTOR_PARENT_CLASSNAME} ${DBUS_ADAPTOR_BASENAME} ${DBUS_ADAPTOR_CLASSNAME})
			else()
				qt5_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_PARENT} ${DBUS_ADAPTOR_PARENT_CLASSNAME} ${DBUS_ADAPTOR_BASENAME})
			endif()
		else()
			qt5_add_dbus_adaptor(${PROJECT_NAME}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_PARENT} ${DBUS_ADAPTOR_PARENT_CLASSNAME})
		endif()
	endforeach()

	foreach(DBUS_INTERFACE ${${PROJECT_NAME}_DBUS_INTERFACES})
		get_source_file_property(DBUS_INTERFACE_BASENAME ${DBUS_INTERFACE} BASENAME)
		qt5_add_dbus_interface(${PROJECT_NAME}_DBUS_INTERFACE_FILES ${DBUS_INTERFACE} ${DBUS_INTERFACE_BASENAME})
	endforeach()
endif()


set(${PROJECT_NAME}_HEADERS
	${${PROJECT_NAME}_MAIN_HEADER}
	${${PROJECT_NAME}_CPP_HEADERS}
	${${PROJECT_NAME}_QT_HEADERS}
)

set(${PROJECT_NAME}_TRANSLATABLE
	${${PROJECT_NAME}_SOURCES}
	${${PROJECT_NAME}_HEADERS}
	${${PROJECT_NAME}_FORMS}
)

if(${QT_VERSION_MAJOR} EQUAL 4)
	qt4_wrap_cpp(${PROJECT_NAME}_MOC_FILES ${${PROJECT_NAME}_QT_HEADERS})
	qt4_add_resources(${PROJECT_NAME}_RESOURCE_FILES ${${PROJECT_NAME}_RESOURCES})
	qt4_wrap_ui(${PROJECT_NAME}_FORM_FILES ${${PROJECT_NAME}_FORMS})
	qt4_create_translation(${${PROJECT_NAME}_QM_FILES} ${${PROJECT_NAME}_TRANSLATABLE} ${${PROJECT_NAME}_TRANSLATIONS})
elseif(${QT_VERSION_MAJOR} EQUAL 5)
	qt5_wrap_cpp(${PROJECT_NAME}_MOC_FILES ${${PROJECT_NAME}_QT_HEADERS})
	qt5_add_resources(${PROJECT_NAME}_RESOURCE_FILES ${${PROJECT_NAME}_RESOURCES})
	qt5_wrap_ui(${PROJECT_NAME}_FORM_FILES ${${PROJECT_NAME}_FORMS})
	qt5_create_translation(${${PROJECT_NAME}_QM_FILES} ${${PROJECT_NAME}_TRANSLATABLE} ${${PROJECT_NAME}_TRANSLATIONS})
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

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_ALL_FILES})

if(${QT_VERSION_MAJOR} EQUAL 4)
	target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})
elseif(${QT_VERSION_MAJOR} EQUAL 5)
	qt5_use_modules(${PROJECT_NAME} Core Widgets DBus)
endif()
