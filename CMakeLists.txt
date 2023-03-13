cmake_minimum_required(VERSION 3.22) # TODO: increase to 3.25

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include("print_variables")
include("third-party")
include("version")


project(knp)

option(KNP_BUILD_TESTS "Build tests" ON)
option(KNP_ENABLE_COVERAGE "Enable coverage test" ON)
option(KNP_BUILD_DOCUMENTATION "Build doxygen auto documentation" ON)
option(KNP_MAINTAINER_BUILD "Build for maintainer, but not for the development purposes" OFF)

if(NOT KNP_MAINTAINER_BUILD)
    execute_process(COMMAND pre-commit install
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    execute_process(COMMAND pre-commit install -t commit-msg
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_TIDY_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/knp/.clang-tidy" CACHE PATH  "Clang-tidy config file")
set(DOXYFILE_IN "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")

# Make cache variables for install destinations.
include(GNUInstallDirs)

file(GLOB THIRD_PARTY_INCLUDES LIST_DIRECTORIES true "third-party/**/include")
include_directories(third-party ${THIRD_PARTY_INCLUDES})

add_third_party(googletest)
add_third_party(spdlog)
add_third_party(pybind11)
add_third_party(pcm)

add_subdirectory(knp)
