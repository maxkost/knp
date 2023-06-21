cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-cpu-multi-threaded-backend VERSION "${KNP_VERSION}" LANGUAGES C CXX)

include(GNUInstallDirs)
include(clang-tidy)
include(knp-functions)

find_package(Boost 1.66.0 REQUIRED)

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/knp/backends/cpu-multi-threaded/*.h)
source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers REGULAR_EXPRESSION "include/.*")

knp_add_library("${PROJECT_NAME}"
    SHARED
    impl/backend.cpp

    impl/blifat_population.h
    impl/blifat_population.cpp

    impl/delta_synapse_projection.h
    impl/delta_synapse_projection.cpp

    ${${PROJECT_NAME}_headers}
)

add_library(KNP::Core::Backend::CPUMultiThreaded ALIAS "${PROJECT_NAME}")

target_link_libraries("${PROJECT_NAME}" PRIVATE Boost::headers spdlog::spdlog_header_only)
target_link_libraries("${PROJECT_NAME}" PUBLIC KNP::Core KNP::Devices::CPU)
