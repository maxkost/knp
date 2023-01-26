#
# CMake version reading functions. Artiom N.(cl)2022
#

function(read_version filename)
    file(READ "${filename}" VERSION_FILE_RAW)
    string(STRIP "${VERSION_FILE_RAW}" VERSION_FILE)
    string(REGEX MATCHALL "\s*[0-9]+\.[0-9]+\.[0-9]+$" VER_LIST "${version_string}")

    set(PROJECT_VERSION ${VER_LIST} CACHE STRING
        "Version determined from \"${filename}\"" FORCE
    )
    message(STATUS "Project version: ${PROJECT_VERSION}")
endfunction()


function(determine_version_with_git)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/GetGitRevisionDescription.cmake)
    git_describe(VERSION ${ARGN})
    string(FIND ${VERSION} "." VALID_VERSION)
    if(VALID_VERSION EQUAL -1)
        message(WARNING "Version string ${VERSION} retrieved with git describe is invalid")
        return()
    endif()
    message(STATUS "${MSG_PREFIX} Version string determined with git describe: ${VERSION}")
    # Parse the version information into pieces.
    string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
    string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")
    set(ETL_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

    set(ETL_VERSION ${ETL_VERSION} CACHE STRING
        "ETL version determined with git describe" FORCE
    )
    message(STATUS "${MSG_PREFIX} Determined ETL version ${ETL_VERSION} from the git tag")
endfunction()

#
#include(CMakePackageConfigHelpers)
#
#write_basic_package_version_file(
#        "utilsConfigVersion.cmake"
#        VERSION ${PROJECT_VERSION}
#        COMPATIBILITY AnyNewerVersion
#)
#
#configure_package_config_file(
#        "${PROJECT_SOURCE_DIR}/cmake/utilsConfig.cmake.in"
#        "${PROJECT_BINARY_DIR}/utilsConfig.cmake"
#        INSTALL_DESTINATION
#        ${CMAKE_INSTALL_LIBDIR}/cmake/utils
#)
#
#install(
#        FILES
#        ${PROJECT_BINARY_DIR}/utilsConfig.cmake
#        ${CMAKE_CURRENT_BINARY_DIR}/utilsConfigVersion.cmake
#        DESTINATION
#        ${CMAKE_INSTALL_LIBDIR}/cmake/utils
#)
