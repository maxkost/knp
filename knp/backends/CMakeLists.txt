cmake_minimum_required(VERSION 3.25)

add_subdirectory(cpu)

file(GLOB backends LIST_DIRECTORIES true "[a-zA-Z0-9_]*-backend")

if(NOT backends)
    message(WARNING "No non-CPU backend was found.")
    return()
endif()

foreach(backend ${backends})
    message(STATUS "Adding backend \"${backend}\"...")
    get_filename_component(BACKEND_NAME "${backend}" NAME)
    add_subdirectory("${backend}")
endforeach()
