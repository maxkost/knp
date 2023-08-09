cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-synapse-traits LANGUAGES C CXX)

include(knp-functions)

add_library("${PROJECT_NAME}"
    INTERFACE
    include/knp/synapse-traits/type_traits.h
    include/knp/synapse-traits/delta.h
    include/knp/synapse-traits/all_traits.h
    include/knp/synapse-traits/output_types.h
    include/knp/synapse-traits/stdp.h
    include/knp/synapse-traits/stdp_add_rule.h
)
add_library(KNP::Synapse::Traits ALIAS "${PROJECT_NAME}")

target_include_directories("${PROJECT_NAME}" INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries("${PROJECT_NAME}" INTERFACE KNP::Meta)
