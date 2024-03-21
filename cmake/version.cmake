#
# CMake version reading functions. Artiom N.(cl)2022
#

include_guard(GLOBAL)


function(read_version_file filename)
    message(STATUS "Reading version file...")
    file(READ "${filename}" VERSION_FILE_RAW)

    string(STRIP "${VERSION_FILE_RAW}" KNP_VERSION)
    set(KNP_VERSION ${KNP_VERSION} PARENT_SCOPE)

    message(STATUS "Version from file: ${KNP_VERSION}")
endfunction()


function(determine_version_with_git)
    message(STATUS "Determining version from Git...")
    execute_process(COMMAND git describe --tags
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_EXECUTION_RESULT
            OUTPUT_VARIABLE GIT_PKG_VERSION_FULL
            ERROR_VARIABLE GIT_VERSION_ERROR
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(GIT_EXECUTION_RESULT)
        message(STATUS "Could not retrieve version from command 'git describe --tags'")
        return()
    endif()

    string(FIND ${VERSION} "." VALID_VERSION)
    if(VALID_VERSION EQUAL -1)
        message(WARNING "Version string ${VERSION} retrieved with git describe is invalid")
        return()
    endif()
    message(STATUS "Version string determined with git describe: ${VERSION}")
endfunction()


function(get_knp_version)
    determine_version_with_git()

    if (NOT KNP_VERSION)
        read_version_file("${CMAKE_SOURCE_DIR}/VERSION")
    endif()

    message(STATUS "KNP version: ${KNP_VERSION}")

    if (NOT KNP_VERSION)
        message(WARNING "KNP version can't be determined!")
        return()
    endif()

    # Parse the version information into pieces.
    string(REGEX REPLACE "v?([0-9]+\\.[0-9]+(\\.[0-9]+)?).*"  "\\1"
           KNP_VERSION "${KNP_VERSION}")

    set(KNP_VERSION "${KNP_VERSION}" PARENT_SCOPE)

    # set(KNP_VERSION ${KNP_VERSION} CACHE STRING "KNP version" FORCE)
endfunction()
