add_subdirectory(razormount)
add_subdirectory(razorqt)
add_subdirectory(razorqxt)
add_subdirectory(qtxdg)

add_dependencies(razorqt qtxdg razorqxt razormount)

find_package(Doxygen)
if(DOXYGEN_FOUND)

    add_custom_target(doc
        echo "doc meta target started"
        DEPENDS doc_qtxdg doc_razormount doc_razorqt doc_razorqxt
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

endif(DOXYGEN_FOUND)
