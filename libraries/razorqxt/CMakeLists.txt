include(RazorLibSuffix)


# our Qxt library
set ( razorqxt_HDRS
    qxtglobal.h
    qxtglobalshortcut.h
    )
    
set ( razorqxt_SRC
    qxtglobal.cpp
    qxtglobalshortcut.cpp
    qxtglobalshortcut_x11.cpp
    )

QT4_WRAP_CPP(qxt_MOCS qxtglobalshortcut.h)

add_library( razorqxt SHARED ${razorqxt_SRC} ${qxt_MOCS}  ${QM_FILES})

target_link_libraries( razorqxt  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                                 ${X11_X11_LIB} )

set_target_properties(razorqxt PROPERTIES
  VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
  SOVERSION ${MAJOR_VERSION}
)

install(TARGETS razorqxt DESTINATION lib${LIB_SUFFIX})
install(FILES ${razorqxt_HDRS} DESTINATION include/razorqxt)



find_package(Doxygen)
if(DOXYGEN_FOUND)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY
    )
    add_custom_target(doc_razorqxt
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

endif(DOXYGEN_FOUND)

