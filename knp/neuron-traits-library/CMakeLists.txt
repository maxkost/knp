cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-neuron-traits LANGUAGES C CXX)

include(knp-functions)

add_library("${PROJECT_NAME}"
    INTERFACE
    include/knp/neuron-traits/type_traits.h
    include/knp/neuron-traits/blifat.h
    include/knp/neuron-traits/all_traits.h
)
add_library(KNP::Neuron::Traits ALIAS "${PROJECT_NAME}")
target_include_directories("${PROJECT_NAME}" INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

#set_property(
#    TARGET "${PROJECT_NAME}" PROPERTY
#    EXPORT_NAME neuron_traits
#)
