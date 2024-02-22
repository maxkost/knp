cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp_python_framework LANGUAGES C CXX)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(Boost 1.66.0 COMPONENTS python REQUIRED)

include(knp-functions)

knp_add_python_module(core LINK_LIBRARIES KNP::Core)
knp_add_python_module(neuron_traits LINK_LIBRARIES KNP::Neuron::Traits)
knp_add_python_module(synapse_traits LINK_LIBRARIES KNP::Synapse::Traits)
knp_add_python_module(base_framework LINK_LIBRARIES KNP::BaseFramework::Core)

#add_custom_command(TARGET module2 POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:module2>
#        "${CMAKE_SOURCE_DIR}/awesome_package"
#        )
