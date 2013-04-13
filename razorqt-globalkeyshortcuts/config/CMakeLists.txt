set(PROJECT_NAME razor-config-globalkeyshortcuts)
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
	main_window.cpp
	actions.cpp
	default_model.cpp
	shortcut_selector.cpp
	../src/meta_types.cpp
	edit_action_dialog.cpp
	shortcut_delegate.cpp
)

set(${PROJECT_NAME}_FORMS
	main_window.ui
	edit_action_dialog.ui
)

set(${PROJECT_NAME}_RESOURCES
)

set(${PROJECT_NAME}_TRANSLATIONS
)

set(${PROJECT_NAME}_CPP_HEADERS
	../src/meta_types.h
)

set(${PROJECT_NAME}_QT_HEADERS
	main_window.h
	actions.h
	default_model.h
	shortcut_selector.h
	edit_action_dialog.h
	shortcut_delegate.h
)

set(${PROJECT_NAME}_DBUS_ADAPTERS
)

set(${PROJECT_NAME}_DBUS_PROXYS
	../src/org.razorqt.global_key_shortcuts.daemon.xml
)
set_source_files_properties(../src/org.razorqt.global_key_shortcuts.daemon.xml PROPERTIES BASENAME org.razorqt.global_key_shortcuts.daemon)
set_source_files_properties(../src/org.razorqt.global_key_shortcuts.daemon.xml PROPERTIES INCLUDE ../src/meta_types.h)



set(${PROJECT_NAME}_HEADERS
	${${PROJECT_NAME}_CPP_HEADERS}
	${${PROJECT_NAME}_QT_HEADERS}
)

set(${PROJECT_NAME}_TRANSLATABLE
	${${PROJECT_NAME}_SOURCES}
	${${PROJECT_NAME}_HEADERS}
	${${PROJECT_NAME}_FORMS}
)



find_package(Qt4 COMPONENTS QtCore QtGui QtDBus)

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

# Translations **********************************
razor_translate_ts(${PROJECT_NAME}_QM_FILES
	SOURCES
	${${PROJECT_NAME}_TRANSLATABLE}
)

razor_translate_desktop(${PROJECT_NAME}_DESKTOP_FILES
	SOURCES
	${PROJECT_NAME}.desktop.in
)
#************************************************



set(${PROJECT_NAME}_GENERATED_FILES
	${${PROJECT_NAME}_MOC_FILES}
	${${PROJECT_NAME}_FORM_FILES}
	${${PROJECT_NAME}_RESOURCE_FILES}
	${${PROJECT_NAME}_QM_FILES}
	${${PROJECT_NAME}_DBUS_INTERFACE_FILES}
	${${PROJECT_NAME}_DBUS_ADAPTOR_FILES}
	${${PROJECT_NAME}_DESKTOP_FILES}
)

set(${PROJECT_NAME}_ALL_FILES
	${${PROJECT_NAME}_SOURCES}
	${${PROJECT_NAME}_HEADERS}
	${${PROJECT_NAME}_GENERATED_FILES}
)



add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_ALL_FILES})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
