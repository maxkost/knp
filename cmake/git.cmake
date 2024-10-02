include_guard(GLOBAL)


find_package(Git QUIET)


if(NOT Git_FOUND)
    find_package(Git REQUIRED)
    message("Git found: ${GIT_EXECUTABLE}.")
endif()


function(add_git_submodule dir)
    # Add a Git submodule directory to CMake, assuming the
    # Git submodule directory is a CMake project.

    set(project_list "${dir}/CMakeLists.txt")

    if(NOT EXISTS "${project_list}")
        message(STATUS "Adding submodule \"${dir}\"...")
        if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
            execute_process(COMMAND "${GIT_EXECUTABLE}" submodule update --init --recursive --depth 1 -- "${dir}"
                            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                            COMMAND_ERROR_IS_FATAL ANY)
        endif()
    endif()

    if(EXISTS "${project_list}")
        # get_filename_component(build_dir "${dir}" NAME)
        set(CMAKE_VERIFY_INTERFACE_HEADER_SETS OFF)
        add_subdirectory("${dir}" EXCLUDE_FROM_ALL)
    endif()

endfunction()
