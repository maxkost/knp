cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp_python_framework LANGUAGES C CXX)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(Boost 1.66.0 COMPONENTS python REQUIRED)

include(knp-functions)

set(KNP_PYTHON_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}")

knp_add_python_module(core
        LINK_LIBRARIES KNP::Core)
knp_add_python_module(neuron_traits
        LINK_LIBRARIES KNP::Neuron::Traits)
knp_add_python_module(synapse_traits
        LINK_LIBRARIES KNP::Synapse::Traits)
knp_add_python_module(base_framework
        LINK_LIBRARIES KNP::BaseFramework::Core)

add_custom_target("${PROJECT_NAME}-copy-python-tests" ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/tests" "${KNP_PYTHON_OUTPUT_DIRECTORY}/tests")
add_custom_target("${PROJECT_NAME}-copy-common-python-files" ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/packaging/python" "${KNP_PYTHON_OUTPUT_DIRECTORY}/knp")

find_program(MYPY_STUBGEN stubgen REQUIRED)
get_property(KNP_PYTHON_TARGETS DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)

add_custom_target("${PROJECT_NAME}-generate-python-interface-files" ALL
        WORKING_DIRECTORY "${KNP_PYTHON_OUTPUT_DIRECTORY}"
        COMMAND ${MYPY_STUBGEN} -p "knp" -o "." --include-private
        DEPENDS ${KNP_PYTHON_TARGETS})

get_property(KNP_PYTHON_TARGETS DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)

if(KNP_BUILD_TESTS)
    add_test(NAME Pytest
            WORKING_DIRECTORY "${KNP_PYTHON_OUTPUT_DIRECTORY}"
            COMMAND ${Python_EXECUTABLE} -m pytest)
endif()
