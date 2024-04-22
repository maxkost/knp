#
# CMake third-party support functions. Artiom N.(cl)2022
#

include_guard(GLOBAL)

include(CPM)
# include("git")


function(add_third_party module_name)
#    add_git_submodule("${CMAKE_CURRENT_SOURCE_DIR}/third-party/${module_name}")
    set(CPM_USE_LOCAL_PACKAGES ON)

    if(NOT "${module_name}" STREQUAL NAME)
        cpm_parse_add_package_single_arg("${module_name};${ARGN}" ARGN)
        # The shorthand syntax implies EXCLUDE_FROM_ALL and SYSTEM
        list(GET ARGN 1 _repo_name)
        get_filename_component(_repo_name ${_repo_name} NAME)
        CPMADDPackage(${ARGN}
                      EXCLUDE_FROM_ALL YES
                      SYSTEM YES
                      GIT_SHALLOW YES
                      SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third-party/${_repo_name}")
    else()
        CPMADDPackage("${module_name}" ${ARGN}
                      EXCLUDE_FROM_ALL YES
                      SYSTEM YES
                      GIT_SHALLOW YES
                      SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third-party/${module_name}")
    endif()
endfunction()
