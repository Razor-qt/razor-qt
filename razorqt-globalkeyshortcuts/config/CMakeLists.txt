set(PROJECT razor-config-globalkeyshortcuts)
project(${PROJECT})

cmake_minimum_required(VERSION 2.6)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()



include_directories(
	"${PROJECT_SOURCE_DIR}"
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_SOURCE_DIR}/libraries/"
)



set(${PROJECT}_SOURCES
	main.cpp
	main_window.cpp
	actions.cpp
	default_model.cpp
	shortcut_selector.cpp
	../src/meta_types.cpp
	edit_action_dialog.cpp
)

set(${PROJECT}_FORMS
	main_window.ui
	edit_action_dialog.ui
)

set(${PROJECT}_RESOURCES
)

set(${PROJECT}_TRANSLATIONS
)

set(${PROJECT}_CPP_HEADERS
	../src/meta_types.h
)

set(${PROJECT}_QT_HEADERS
	main_window.h
	actions.h
	default_model.h
	shortcut_selector.h
	edit_action_dialog.h
)

set(${PROJECT}_DBUS_ADAPTERS
)

set(${PROJECT}_DBUS_PROXYS
	../src/org.razorqt.global_action.daemon.xml
)
set_source_files_properties(../src/org.razorqt.global_action.daemon.xml PROPERTIES BASENAME org.razorqt.global_action.daemon)
set_source_files_properties(../src/org.razorqt.global_action.daemon.xml PROPERTIES INCLUDE ../src/meta_types.h)



set(${PROJECT}_HEADERS
	${${PROJECT}_CPP_HEADERS}
	${${PROJECT}_QT_HEADERS}
)

set(${PROJECT}_TRANSLATABLE
	${${PROJECT}_SOURCES}
	${${PROJECT}_HEADERS}
	${${PROJECT}_FORMS}
)



find_package(Qt4 COMPONENTS QtCore QtGui QtDBus)

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

# Translations **********************************
razor_translate_ts(${PROJECT}_QM_FILES
	SOURCES
	${${PROJECT}_TRANSLATABLE}
)

razor_translate_desktop(${PROJECT}_DESKTOP_FILES
	SOURCES
	${PROJECT}.desktop.in
)
#************************************************



set(${PROJECT}_GENERATED_FILES
	${${PROJECT}_MOC_FILES}
	${${PROJECT}_FORM_FILES}
	${${PROJECT}_RESOURCE_FILES}
	${${PROJECT}_QM_FILES}
	${${PROJECT}_DBUS_INTERFACE_FILES}
	${${PROJECT}_DBUS_ADAPTOR_FILES}
	${${PROJECT}_DESKTOP_FILES}
)

set(${PROJECT}_ALL_FILES
	${${PROJECT}_SOURCES}
	${${PROJECT}_HEADERS}
	${${PROJECT}_GENERATED_FILES}
)



add_executable(${PROJECT} ${${PROJECT}_ALL_FILES})
target_link_libraries(${PROJECT} ${QT_LIBRARIES})

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
