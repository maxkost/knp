#
# CMake third-party support functions. Artiom N.(cl)2022
#

include("git")

function(add_third_party module_name)
    add_git_submodule("${CMAKE_CURRENT_SOURCE_DIR}/third-party/${module_name}")
endfunction()
