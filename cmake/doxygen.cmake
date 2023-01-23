#
# Artiom N.(cl)2023
#

include_guard(GLOBAL)

find_package(Doxygen)


macro(add_doxygen target_name doxyfile_in)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/doxyfile")
    configure_file("${doxyfile_in}" "${DOXYFILE}" @ONLY)
    message("Doxygen build started.")

    add_custom_target("${target_name}"
                      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)

#    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION     share/doc)
endmacro()
