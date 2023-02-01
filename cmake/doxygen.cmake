#
# Artiom N.(cl)2023
#

include_guard(GLOBAL)

find_package(Doxygen REQUIRED)


macro(add_doxygen target_name doxyfile_in)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(_s_auto_list PROJECT_BRIEF OUTPUT_DIRECTORY INPUT LANGUAGE EXAMPLE_PATH PROJECT_LOGO
        GENERATE_TAGFILE BUNDLE_ID PUBLISHER_NAME SOURCE_BROWSER VERBATIM_HEADERS)
    set(_m_auto_list EXCLUDE_PATTERNS EXTRA_FILES PREDEFINED TAGFILES STRIP_FROM_INC_PATH)

    cmake_parse_arguments(
        "PARSED_ARGS"
        ""
        "PROJECT_VERSION;${_s_auto_list}"
        "${_m_auto_list}"
        ${ARGN}
    )

    set(DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/doxyfile")
    set(DOXYGEN_PROJECT_NAME "${target_name}")
    set(DOXYGEN_PROJECT_NUMBER "${PARSED_ARGS_PROJECT_VERSION}")

    set(DOXYGEN_LANGUAGE "English")
    set(DOXYGEN_SOURCE_BROWSER YES)
    set(DOXYGEN_VERBATIM_HEADERS YES)

    foreach(i_var IN LISTS _s_auto_list _m_auto_list)
        if(PARSED_ARGS_${i_var})
            set(DOXYGEN_${i_var} "${PARSED_ARGS_${i_var}}")
        endif()
    endforeach()

    foreach(i_var IN LISTS _m_auto_list)
        if(PARSED_ARGS_${i_var})
            string(REPLACE ";" " \\\n    " DOXYGEN_${i_var} "${PARSED_ARGS_${i_var}}")
            set(DOXYGEN_${i_var} "\\\n    ${DOXYGEN_${i_var}}")
        endif()
    endforeach()

    get_filename_component(DOXYGEN_SRCDIR "${doxyfile_in}" DIRECTORY)

    configure_file("${doxyfile_in}" "${DOXYFILE}" @ONLY)
    message("Doxygen build started.")

    # Old variant:
    add_custom_target("${target_name}"
                      COMMAND Doxygen::doxygen "${DOXYFILE}"
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)
    # Config generation, new CMake variant:
    # unset(DOXYGEN_INPUT)
    # doxygen_add_docs("${target_name}"
    #                 ${INPUT}
    #                 ALL
    #                 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    #                 COMMENT "Generating API documentation with Doxygen"
    # )
#    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION     share/doc)
endmacro()
