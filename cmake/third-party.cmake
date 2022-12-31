#
# CMake third-party support functions. Artiom N.(cl)2022
#

include("git")

function(add_third_party module_name)
    add_git_submodule("third-party/${module_name}")
endfunction()
