cmake_minimum_required(VERSION 3.22) # TODO: increase to 3.25

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include("version")
include("third-party")

project(knp)

option(BUILD_TESTS "Build tests" ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Make cache variables for install destinations.
include(GNUInstallDirs)

file(GLOB THIRD_PARTY_INCLUDES LIST_DIRECTORIES true "third-party/**/include")
include_directories(${THIRD_PARTY_INCLUDES})

add_third_party(googletest)
add_third_party(spdlog)
add_third_party(pybind11)

add_subdirectory(knp)
