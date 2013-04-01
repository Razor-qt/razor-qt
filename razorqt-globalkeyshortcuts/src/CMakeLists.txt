set(PROJECT razor-globalkeyshortcuts)
project(${PROJECT})

cmake_minimum_required(VERSION 2.6)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()



include_directories(
	"${PROJECT_SOURCE_DIR}"
	"${CMAKE_CURRENT_BINARY_DIR}"
)



set(${PROJECT}_SOURCES
	main.cpp
	core.cpp
	daemon_adaptor.cpp
	native_adaptor.cpp
	dbus_proxy.cpp
	client_proxy.cpp
	log_target.cpp
	pipe_utils.cpp
	string_utils.cpp
	base_action.cpp
	method_action.cpp
	dbus_action.cpp
	command_action.cpp
	meta_types.cpp
)

set(${PROJECT}_CPP_HEADERS
	log_target.h
	pipe_utils.h
	string_utils.h
	base_action.h
	method_action.h
	dbus_action.h
	command_action.h
	meta_types.h
)

set(${PROJECT}_QT_HEADERS
	core.h
	dbus_proxy.h
	client_proxy.h
	daemon_adaptor.h
	native_adaptor.h
)

set(${PROJECT}_FORMS
)

set(${PROJECT}_RESOURCES
)

set(${PROJECT}_TRANSLATIONS
)

set(${PROJECT}_DBUS_ADAPTERS
	org.razorqt.global_action.daemon.xml
	org.razorqt.global_action.native.xml
)

set_source_files_properties(org.razorqt.global_action.daemon.xml PROPERTIES PARENT daemon_adaptor.h)
set_source_files_properties(org.razorqt.global_action.daemon.xml PROPERTIES PARENT_CLASSNAME DaemonAdaptor)
set_source_files_properties(org.razorqt.global_action.daemon.xml PROPERTIES BASENAME org.razorqt.global_action.daemon)
set_source_files_properties(org.razorqt.global_action.daemon.xml PROPERTIES CLASSNAME OrgRazorqtGlobalActionDaemonAdaptor)

set_source_files_properties(org.razorqt.global_action.native.xml PROPERTIES PARENT native_adaptor.h)
set_source_files_properties(org.razorqt.global_action.native.xml PROPERTIES PARENT_CLASSNAME NativeAdaptor)
set_source_files_properties(org.razorqt.global_action.native.xml PROPERTIES BASENAME org.razorqt.global_action.native)
set_source_files_properties(org.razorqt.global_action.native.xml PROPERTIES CLASSNAME OrgRazorqtGlobalActionNativeAdaptor)

set(${PROJECT}_DBUS_PROXYS
	org.freedesktop.DBus.xml
	org.razorqt.global_action.client.xml
)

set_source_files_properties(org.freedesktop.DBus.xml PROPERTIES BASENAME org.freedesktop.DBus)

set_source_files_properties(org.razorqt.global_action.client.xml PROPERTIES BASENAME org.razorqt.global_action.client)



set(${PROJECT}_HEADERS
	${${PROJECT}_CPP_HEADERS}
	${${PROJECT}_QT_HEADERS}
)

set(${PROJECT}_TRANSLATABLE
	${${PROJECT}_SOURCES}
	${${PROJECT}_HEADERS}
	${${PROJECT}_FORMS}
)



find_package(X11)
include_directories(${X11_INCLUDE_DIR})


find_package(Qt4 COMPONENTS QtCore QtDBus)

include(${QT_USE_FILE})

foreach(DBUS_ADAPTOR ${${PROJECT}_DBUS_ADAPTERS})
	get_source_file_property(DBUS_ADAPTOR_INCLUDE ${DBUS_ADAPTOR} PARENT)
	get_source_file_property(DBUS_ADAPTOR_PARENT_CLASSNAME ${DBUS_ADAPTOR} PARENT_CLASSNAME)
	get_source_file_property(DBUS_ADAPTOR_BASENAME ${DBUS_ADAPTOR} BASENAME)
	get_source_file_property(DBUS_ADAPTOR_CLASSNAME ${DBUS_ADAPTOR} CLASSNAME)
	if(DBUS_ADAPTOR_BASENAME)
		if(DBUS_ADAPTOR_CLASSNAME)
			qt4_add_dbus_adaptor(${PROJECT}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_INCLUDE} ${DBUS_ADAPTOR_PARENT_CLASSNAME} ${DBUS_ADAPTOR_BASENAME} ${DBUS_ADAPTOR_CLASSNAME})
		else()
			qt4_add_dbus_adaptor(${PROJECT}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_INCLUDE} ${DBUS_ADAPTOR_PARENT_CLASSNAME} ${DBUS_ADAPTOR_BASENAME})
		endif()
	else()
		qt4_add_dbus_adaptor(${PROJECT}_DBUS_ADAPTOR_FILES ${DBUS_ADAPTOR} ${DBUS_ADAPTOR_INCLUDE} ${DBUS_ADAPTOR_PARENT_CLASSNAME})
	endif()
endforeach()

foreach(DBUS_INTERFACE ${${PROJECT}_DBUS_PROXYS})
	get_source_file_property(DBUS_INTERFACE_BASENAME ${DBUS_INTERFACE} BASENAME)
	qt4_add_dbus_interface(${PROJECT}_DBUS_INTERFACE_FILES ${DBUS_INTERFACE} ${DBUS_INTERFACE_BASENAME})
endforeach()

qt4_wrap_cpp(${PROJECT}_MOC_FILES ${${PROJECT}_QT_HEADERS})
qt4_add_resources(${PROJECT}_RESOURCE_FILES ${${PROJECT}_RESOURCES})
if(COMMAND qt4_wrap_ui)
	qt4_wrap_ui(${PROJECT}_FORM_FILES ${${PROJECT}_FORMS})
endif()
#if(COMMAND qt4_create_translation)
#	qt4_create_translation(${${PROJECT}_QM_FILES} ${${PROJECT}_TRANSLATABLE} ${${PROJECT}_TRANSLATIONS})
#endif()



set(${PROJECT}_GENERATED_FILES
	${${PROJECT}_MOC_FILES}
	${${PROJECT}_FORM_FILES}
	${${PROJECT}_RESOURCE_FILES}
	${${PROJECT}_QM_FILES}
	${${PROJECT}_DBUS_INTERFACE_FILES}
	${${PROJECT}_DBUS_ADAPTOR_FILES}
)

set(${PROJECT}_ALL_FILES
	${${PROJECT}_SOURCES}
	${${PROJECT}_HEADERS}
	${${PROJECT}_GENERATED_FILES}
)



add_executable(${PROJECT} ${${PROJECT}_ALL_FILES})
target_link_libraries(${PROJECT} ${X11_LIBRARIES} ${QT_LIBRARIES})

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
