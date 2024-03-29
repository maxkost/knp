cmake_minimum_required(VERSION 3.25)
project(knp-documentation
        VERSION "${KNP_VERSION}"
        DESCRIPTION "Documentation: user guide and reference")

include(GNUInstallDirs)
include(knp-functions)

knp_get_component_name("${PROJECT_NAME}" COMPONENT_NAME)

install(DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/html"
        "${CMAKE_CURRENT_SOURCE_DIR}/pdf"
        COMPONENT "${COMPONENT_NAME}"
        TYPE DOC)
#        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/knp/html")

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/html/index.html" FILE_CONTENTS)
string(REPLACE "../build/doc_doxygen/html/index.html" "reference/index.html" FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/index.html" "${FILE_CONTENTS}")

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/index.html"
        COMPONENT "${COMPONENT_NAME}"
        TYPE DOC
        RENAME "html/index.html")

knp_packaging_set_parameters("${COMPONENT_NAME}" "${PROJECT_NAME}"
        DESCRIPTION "${PROJECT_DESCRIPTION}"
        DEPENDS framework-dev)
#set(CPACK_COMPONENT_DOCUMENTATION_GROUP "knp-sdk" PARENT_SCOPE)

knp_get_component_var_name("${COMPONENT_NAME}" COMPONENT_VAR)

set(CPACK_DEBIAN_${COMPONENT_VAR}_PACKAGE_ARCHITECTURE "all" CACHE STRING "${PROJECT_NAME} architecture")
set(CPACK_DEBIAN_${COMPONENT_VAR}_PACKAGE_PROVIDES "documentation  (= ${KNP_VERSION})" CACHE STRING "${PROJECT_NAME} provides")
set(CPACK_DEBIAN_${COMPONENT_VAR}_PACKAGE_SHLIBDEPS OFF CACHE BOOL "")
