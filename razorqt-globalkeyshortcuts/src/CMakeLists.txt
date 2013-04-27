set(PROJECT_NAME razor-globalkeyshortcuts)
project(${PROJECT_NAME})

cmake_minimum_required(VERSION 2.6)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()



include_directories(
	"${PROJECT_SOURCE_DIR}"
	"${CMAKE_CURRENT_BINARY_DIR}"
)



set(${PROJECT_NAME}_SOURCES
	main.cpp
	core.cpp
	daemon_adaptor.cpp
	native_adaptor.cpp
	client_proxy.cpp
	log_target.cpp
	pipe_utils.cpp
	string_utils.cpp
	base_action.cpp
	method_action.cpp
	client_action.cpp
	command_action.cpp
	meta_types.cpp
)

set(${PROJECT_NAME}_CPP_HEADERS
	log_target.h
	pipe_utils.h
	string_utils.h
	base_action.h
	method_action.h
	client_action.h
	command_action.h
	meta_types.h
)

set(${PROJECT_NAME}_QT_HEADERS
	core.h
	client_proxy.h
	daemon_adaptor.h
	native_adaptor.h
)

set(${PROJECT_NAME}_FORMS
)

set(${PROJECT_NAME}_RESOURCES
)

set(${PROJECT_NAME}_TRANSLATIONS
)

set(${PROJECT_NAME}_DBUS_ADAPTERS
	org.razorqt.global_key_shortcuts.daemon.xml
	org.razorqt.global_key_shortcuts.native.xml
)

set_source_files_properties(org.razorqt.global_key_shortcuts.daemon.xml PROPERTIES PARENT daemon_adaptor.h)
set_source_files_properties(org.razorqt.global_key_shortcuts.daemon.xml PROPERTIES PARENT_CLASSNAME DaemonAdaptor)
set_source_files_properties(org.razorqt.global_key_shortcuts.daemon.xml PROPERTIES BASENAME org.razorqt.global_key_shortcuts.daemon)
set_source_files_properties(org.razorqt.global_key_shortcuts.daemon.xml PROPERTIES CLASSNAME OrgRazorqtGlobalActionDaemonAdaptor)

set_source_files_properties(org.razorqt.global_key_shortcuts.native.xml PROPERTIES PARENT native_adaptor.h)
set_source_files_properties(org.razorqt.global_key_shortcuts.native.xml PROPERTIES PARENT_CLASSNAME NativeAdaptor)
set_source_files_properties(org.razorqt.global_key_shortcuts.native.xml PROPERTIES BASENAME org.razorqt.global_key_shortcuts.native)
set_source_files_properties(org.razorqt.global_key_shortcuts.native.xml PROPERTIES CLASSNAME OrgRazorqtGlobalActionNativeAdaptor)

set(${PROJECT_NAME}_DBUS_PROXYS
	org.razorqt.global_key_shortcuts.client.xml
)

set_source_files_properties(org.razorqt.global_key_shortcuts.client.xml PROPERTIES BASENAME org.razorqt.global_key_shortcuts.client)



set(${PROJECT_NAME}_HEADERS
	${${PROJECT_NAME}_CPP_HEADERS}
	${${PROJECT_NAME}_QT_HEADERS}
)

set(${PROJECT_NAME}_TRANSLATABLE
	${${PROJECT_NAME}_SOURCES}
	${${PROJECT_NAME}_HEADERS}
	${${PROJECT_NAME}_FORMS}
)



find_package(X11)
include_directories(${X11_INCLUDE_DIR})


find_package(Qt4 COMPONENTS QtCore QtDBus)

include(${QT_USE_FILE})

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

qt4_wrap_cpp(${PROJECT_NAME}_MOC_FILES ${${PROJECT_NAME}_QT_HEADERS})
qt4_add_resources(${PROJECT_NAME}_RESOURCE_FILES ${${PROJECT_NAME}_RESOURCES})
if(COMMAND qt4_wrap_ui)
	qt4_wrap_ui(${PROJECT_NAME}_FORM_FILES ${${PROJECT_NAME}_FORMS})
endif()
#if(COMMAND qt4_create_translation)
#	qt4_create_translation(${${PROJECT_NAME}_QM_FILES} ${${PROJECT_NAME}_TRANSLATABLE} ${${PROJECT_NAME}_TRANSLATIONS})
#endif()



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
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES} ${QT_LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
