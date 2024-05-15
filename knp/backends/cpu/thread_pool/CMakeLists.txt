cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-thread-pool-library VERSION "${KNP_VERSION}" LANGUAGES C CXX)

find_package(Boost ${KNP_BOOST_MIN_VERSION} REQUIRED)

include(clang-tidy)
include(knp-functions)

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/knp/backends/thread_pool/*.h)

knp_add_library("${PROJECT_NAME}"
    STATIC
    impl/thread_pool_context.cpp
    ${${PROJECT_NAME}_headers}
)
add_library(KNP::Backends::CPU::ThreadPool ALIAS "${PROJECT_NAME}")

source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers FILES ${${PROJECT_NAME}_headers} REGULAR_EXPRESSION "include/.*")

target_include_directories("${PROJECT_NAME}" PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries("${PROJECT_NAME}" PRIVATE Boost::headers spdlog::spdlog)

if(KNP_ENABLE_COVERAGE)
    include(codecoverage)
    append_coverage_compiler_flags()
    link_target_with_gcov("${PROJECT_NAME}")
endif()

# Internal library, used by backends.
# This doesn't need installation.
