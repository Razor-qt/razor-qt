add_subdirectory(razormount)
add_subdirectory(razorqt)
add_subdirectory(razorqxt)
add_subdirectory(qtxdg)

add_dependencies(razorqt qtxdg razorqxt razormount)

find_package(Doxygen)
if(DOXYGEN_FOUND)

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
    add_custom_target(doc
        ${CMAKE_SOURCE_DIR}/scripts/generateDirIndexHtml.sh > ${CMAKE_BINARY_DIR}/docs/index.html
        DEPENDS doc_qtxdg doc_razormount doc_razorqt doc_razorqxt
	COMMENT "Generating API documentation with Doxygen in: ${CMAKE_BINARY_DIR}/docs" VERBATIM
    )

endif(DOXYGEN_FOUND)
