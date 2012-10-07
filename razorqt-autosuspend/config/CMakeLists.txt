project(razor-config-autosuspend)

include_directories ( ${CMAKE_CURRENT_SOURCE_DIR}
                      ${CMAKE_CURRENT_BINARY_DIR}
                      ${CMAKE_SOURCE_DIR}/libraries
                    )

set ( razor-config-autosuspend_SRCS
      main.cpp
      mainwindow.cpp
      generalsettings.cpp
      lidsettings.cpp
      powerlowsettings.cpp
    )

set ( razor-config-autosuspend_UIS
      generalsettings.ui
      lidsettings.ui
      powerlowsettings.ui  
    )

set ( razor-config-autosuspend_RSCS
        razorautosuspend-config.qrc
    )

QT4_ADD_RESOURCES( RSCS
                   ${razor-config-autosuspend_RSCS}
                 )

set(DESKTOP_FILES_IN
    razor-config-autosuspend.desktop.in
)

set ( razor-config-autosuspend_MOCS
      generalsettings.h
      lidsettings.h
      powerlowsettings.h
    )


set ( ICON_FILES icons/laptop-lid.svg
    )

# Translations **********************************
include(RazorTranslate)
razor_translate_ts(QM_FILES
    SOURCES
        ${razor-config-autosuspend_HDRS}
        ${razor-config-autosuspend_SRCS}
        ${razor-config-autosuspend_UIS}
)

razor_translate_desktop(DESKTOP_FILES
    SOURCES
        ${DESKTOP_FILES_IN}
)
#************************************************

QT4_WRAP_CPP( MOCS ${razor-config-autosuspend_MOCS}
            )

QT4_WRAP_UI( UIS ${razor-config-autosuspend_UIS}
           )

add_executable ( ${PROJECT_NAME} 
                 ${razor-config-autosuspend_SRCS} 
                 ${UIS} 
                 ${MOCS} 
                 ${DESKTOP_FILES}
                 ${QM_FILES}
               )

target_link_libraries ( razor-config-autosuspend  
                        razorqt 
                        ${QT_QTCORE_LIBRARY} 
                        ${QT_QTGUI_LIBRARY}  
                      )



install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES   ${DESKTOP_FILES}   DESTINATION share/applications)
install(FILES   ${ICON_FILES}      DESTINATION share/icons/hicolor/scalable/apps)

