cmake_minimum_required(VERSION 3.22) # TODO: increase to 3.25

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(knp-functions)
include(third-party)
include(version)


project(knp)

option(KNP_BUILD_TESTS "Build tests" ON)
option(KNP_ENABLE_COVERAGE "Enable coverage test" ON)
option(KNP_BUILD_DOCUMENTATION "Build doxygen auto documentation" ON)
option(KNP_MAINTAINER_BUILD "Build for maintainer, but not for the development purposes" OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    message(STATUS "Setting release optimization flags...")
    set(CMAKE_C_FLAGS_RELEASE "-march=native -mtune=native -Ofast -fopt-info-vec-optimized -funroll-loops -fomit-frame-pointer -finline -mno-vzeroupper -pipe")
    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
endif()

set(ENABLE_FAST_MATH ON)

if(NOT KNP_MAINTAINER_BUILD)
    execute_process(COMMAND pre-commit install
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    if(ret AND NOT ret EQUAL 0)
        message(FATAL_ERROR "Can't install pre-commit hooks!")
    endif()
    execute_process(COMMAND pre-commit install -t commit-msg
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

if(KNP_BUILD_TESTS)
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=0)
endif()

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/third-party)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/third-party)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/third-party)

set(CMAKE_TIDY_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/knp/.clang-tidy" CACHE PATH  "Clang-tidy config file")
set(DOXYFILE_IN "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")

# Make cache variables for install destinations.
include(GNUInstallDirs)

file(GLOB THIRD_PARTY_INCLUDES LIST_DIRECTORIES true "third-party/**/include")
include_directories(third-party ${THIRD_PARTY_INCLUDES})

include(print-variables)

add_third_party(googletest)

set(SPDLOG_BUILD_PIC ON)
add_third_party(spdlog)
add_third_party(pybind11)
add_third_party(pcm)
set(FLATBUFFERS_BUILD_TESTS OFF)
set(FLATBUFFERS_BUILD_CPP17 ON)
add_third_party(flatbuffers)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(knp)
