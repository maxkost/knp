#
# CMake third-party support functions. Artiom N.(cl)2022
#

include_guard(GLOBAL)

include(CPM)
set(CPM_)
# include("git")


function(add_third_party module_name)
#    add_git_submodule("${CMAKE_CURRENT_SOURCE_DIR}/third-party/${module_name}")
    set(CPM_USE_LOCAL_PACKAGES ON)
    CPMADDPackage("${module_name}" ${ARGN})
endfunction()
