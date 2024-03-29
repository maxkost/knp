cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp_python_framework VERSION "${KNP_VERSION}" LANGUAGES C CXX  # PYTHON
        DESCRIPTION "Kaspersky Neuromorphic Platform Python framework"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

set(Boost_USE_STATIC_LIBS ON)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(Boost ${KNP_BOOST_MIN_VERSION} COMPONENTS python REQUIRED)

include(knp-functions)
include(file-functions)

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
add_custom_target("${PROJECT_NAME}-copy-common-files" ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/packaging/common" "${KNP_PYTHON_OUTPUT_DIRECTORY}")
add_custom_target("${PROJECT_NAME}-copy-common-python-files" ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/packaging/python" "${KNP_PYTHON_OUTPUT_DIRECTORY}/knp")
add_custom_target("${PROJECT_NAME}-copy-common-project-files" ALL
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/AUTHORS"
            "${CMAKE_SOURCE_DIR}/LICENSE.txt"
            "${CMAKE_SOURCE_DIR}/README.md"
            "${CMAKE_SOURCE_DIR}/VERSION"
            "${KNP_PYTHON_OUTPUT_DIRECTORY}")

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

install(DIRECTORY
        "${KNP_PYTHON_OUTPUT_DIRECTORY}/knp"
        COMPONENT "python"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/python3"
        PATTERN "__pycache__" EXCLUDE
        PATTERN "__init__.pyi" EXCLUDE)

set(CPACK_DEBIAN_PYTHON_DESCRIPTION "${PROJECT_DESCRIPTION}" PARENT_SCOPE)
set(CPACK_DEBIAN_PYTHON_PACKAGE_DEPENDS "python3 (>= 3.10)," PARENT_SCOPE)
