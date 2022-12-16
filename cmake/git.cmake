find_package(Git QUIET)


function(add_git_submodule dir)
    # Add a Git submodule directory to CMake, assuming the
    # Git submodule directory is a CMake project.

    set(project_list "${dir}/CMakeLists.txt")

    if(NOT EXISTS "${project_list}")
        message(STATUS "Adding submodule \"${project_list}\"...")
        if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${dir}
                            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                            COMMAND_ERROR_IS_FATAL ANY)
            message(FATAL_ERROR "\"${project_list}\" was not found and Git can't be executed!")
        endif()
    endif()

    add_subdirectory(${dir})
endfunction(add_git_submodule)

