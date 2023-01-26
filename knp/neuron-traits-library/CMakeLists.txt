cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(neuron_traits LANGUAGES C CXX)
add_library("${PROJECT_NAME}" INTERFACE)
add_library(KNP::neuron_traits ALIAS "${PROJECT_NAME}")
target_include_directories("${PROJECT_NAME}" INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

#set_property(
#    TARGET "${PROJECT_NAME}" PROPERTY
#    EXPORT_NAME neuron_traits
#)
