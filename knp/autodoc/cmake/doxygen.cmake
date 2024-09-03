#
# Artiom N.(cl)2023
#

include_guard(GLOBAL)

find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)

#
# Old Doxygen generator.
#

macro(autodoc_add_doxygen target_name doxyfile_in)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(_s_auto_list PROJECT_NAME PROJECT_BRIEF OUTPUT_DIRECTORY INPUT LANGUAGE EXAMPLE_PATH
        GENERATE_TAGFILE BUNDLE_ID PUBLISHER_NAME SOURCE_BROWSER VERBATIM_HEADERS)
    set(_m_auto_list EXCLUDE_PATTERNS EXCLUDE_SYMBOLS EXTRA_FILES PREDEFINED TAGFILES STRIP_FROM_INC_PATH)

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

    add_custom_target("${target_name}"
                      COMMAND Doxygen::doxygen "${DOXYFILE}"
                      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)
endmacro()


#
# Following function duplicated here, because documentation package must be independent
# from the main package components.
#

# Function:          get_files_and_dir
# Description:       Get files and directories of the source directory in two separate lists.
# Param source_dir:  Directory to list.
# output dir_list:   Directories in the source directory.
# output file_list:  Files in the source directory.
function(autodoc_get_files_and_dir source_dir dir_list file_list)
    file(GLOB nodes LIST_DIRECTORIES YES ${source_dir}/*)

    set(_dir_list "")
    set(_file_list "")

    foreach(_node ${nodes})
        if(IS_DIRECTORY "${_node}")
            list(APPEND _dir_list "${_node}")
        else()
            list(APPEND _file_list "${_node}")
        endif()
    endforeach()

    set(${dir_list} ${_dir_list} PARENT_SCOPE)
    set(${file_list} ${_file_list} PARENT_SCOPE)
endfunction(autodoc_get_files_and_dir)
