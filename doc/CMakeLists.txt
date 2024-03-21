cmake_minimum_required(VERSION 3.25)
project(knp-documentation
        VERSION "${KNP_VERSION}"
        DESCRIPTION "Kaspersky Neuromorphic Platform documentation")

install(DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/html"
        COMPONENT "documentation"
        TYPE DOC)
#        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/knp/html")

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/html/index.html" FILE_CONTENTS)
string(REPLACE "../build/doc_doxygen/html/index.html" "reference/index.html" FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/index.html" "${FILE_CONTENTS}")

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/index.html"
        COMPONENT "documentation"
        TYPE DOC
        RENAME "html/index.html")

set(CPACK_DEBIAN_knp-documentation_PACKAGE_ARCHITECTURE "noarch")
