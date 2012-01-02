find_package(PolkitQt-1)
if (NOT POLKITQT-1_LIB_DIR AND NOT POLKITQT-1_INCLUDE_DIR)
    message(WARNING "Qt bindings for policykit are not found. Razor policykit integration won't be built")
else ()
    message(STATUS "PolkitQt-1: ${POLKITQT-1_VERSION_STRING}")
    message(STATUS "      libs: ${POLKITQT-1_CORE_LIBRARY} ${POLKITQT-1_AGENT_LIBRARY}")
    message(STATUS "   include: ${POLKITQT-1_INCLUDE_DIR}")

    # TODO/FIXME: pkg-config for hardcoded paths
    # pkg-config --cflags glib-2.0
    # pkg-config --cflags polkit-agent-1
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/libraries
        /usr/include/glib-2.0 /usr/lib64/glib-2.0/include
        /usr/include/polkit-1 /usr/include/glib-2.0 /usr/lib64/glib-2.0/include
        ${POLKITQT-1_INCLUDE_DIR}
    )

    set(razor-policykit-agent_SRCS
        src/main.cpp
        src/policykitagent.cpp
        src/policykitagentgui.cpp
    )

    set(razor-policykit-agent_H
        src/policykitagent.h
        src/policykitagentgui.h
    )
    
    QT4_WRAP_CPP(razor-policykit-agent_MOCS ${razor-policykit-agent_H})
    QT4_WRAP_UI(razor-policykit-agent_UI src/policykitagentgui.ui)

    add_executable(razor-policykit-agent
        ${razor-policykit-agent_SRCS} ${razor-policykit-agent_MOCS}
        ${razor-policykit-agent_UI}
    )

    target_link_libraries(razor-policykit-agent
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${POLKITQT-1_CORE_LIBRARY} ${POLKITQT-1_AGENT_LIBRARY}
        glib-2.0 gobject-2.0
        qtxdg razorqt
    )
    
    # TODO/FIXME: install!

endif()
