cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-cpu-single-threaded-backend VERSION "${KNP_VERSION}" LANGUAGES C CXX)

include(GNUInstallDirs)
include(clang-tidy)

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/knp/backends/cpu-single-threaded/*.h)
source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers REGULAR_EXPRESSION "include/.*")

add_library("${PROJECT_NAME}"
    impl/backend.cpp
    impl/blifat_population.cpp
    impl/delta_synapse_projection.cpp

    ${${PROJECT_NAME}_headers}
)

add_library(KNP::Core::Backend::CPUSingleThreaded ALIAS "${PROJECT_NAME}")

target_include_directories("${PROJECT_NAME}" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries("${PROJECT_NAME}" PUBLIC spdlog::spdlog_header_only)
target_link_libraries("${PROJECT_NAME}" PUBLIC knp-core KNP::Devices::CPU knp-neuron-traits knp-synapse-traits)
