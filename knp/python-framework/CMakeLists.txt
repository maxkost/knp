cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#define BOOST_PYTHON_STATIC_LIB
#define Py_NO_ENABLE_SHARED

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp_python_framework VERSION "${KNP_VERSION}" LANGUAGES C CXX  # PYTHON
        DESCRIPTION "Kaspersky Neuromorphic Platform Python framework"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

if (WIN32)
    set(Boost_USE_STATIC_LIBS ON)
endif()

if (NOT KNP_PYTHON_FRAMEWORK_BUILD)
    message(STATUS "Python framework building disabled...")
    return()
endif()

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

find_package(Boost ${KNP_BOOST_MIN_VERSION} COMPONENTS)

if (NOT BOOST_PYTHON_FOUND)
    file(GLOB _KNP_PY_VER "${BOOST_LIBRARYDIR}/*boost_python*.[lib|a]")
    if (_KNP_PY_VER)
        list(GET _KNP_PY_VER 0 _KNP_PY_VER)
        string(REGEX REPLACE ".+python([0-9]+).*[\n\r]*" "\\1" _KNP_PY_VER "${_KNP_PY_VER}")
    endif()
    find_package(Boost ${KNP_BOOST_MIN_VERSION} COMPONENTS python${_KNP_PY_VER} REQUIRED)
endif()

include(knp-functions)
include(file-functions)

set(KNP_PYTHON_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}")

knp_add_python_module(core
        LINK_LIBRARIES KNP::Core
        PY_VER ${_KNP_PY_VER})
knp_add_python_module(neuron_traits
        LINK_LIBRARIES KNP::Neuron::Traits
        PY_VER ${_KNP_PY_VER})
knp_add_python_module(synapse_traits
        LINK_LIBRARIES KNP::Synapse::Traits
        PY_VER ${_KNP_PY_VER})
knp_add_python_module(base_framework
        LINK_LIBRARIES KNP::BaseFramework::Core
        PY_VER ${_KNP_PY_VER})

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
        COMMAND "${MYPY_STUBGEN}" -p "knp" -o "." --include-private
        DEPENDS ${KNP_PYTHON_TARGETS})

get_property(KNP_PYTHON_TARGETS DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)

if(KNP_BUILD_TESTS)
    add_test(NAME Pytest
            WORKING_DIRECTORY "${KNP_PYTHON_OUTPUT_DIRECTORY}"
            COMMAND "${Python_EXECUTABLE}" -m pytest)
endif()

install(DIRECTORY
        "${KNP_PYTHON_OUTPUT_DIRECTORY}/knp"
        COMPONENT "python"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/python3"
        PATTERN "__pycache__" EXCLUDE
        PATTERN "__init__.pyi" EXCLUDE
        PATTERN "*.egg-info" EXCLUDE)

knp_packaging_set_parameters("${COMPONENT_NAME}" "${PROJECT_NAME}"
        DESCRIPTION "${PROJECT_DESCRIPTION}"
        DEPENDS framework)

knp_get_component_name("${PROJECT_NAME}" COMPONENT_NAME)
knp_get_component_var_name("${COMPONENT_NAME}" COMPONENT_VAR)
set(CPACK_DEBIAN_${COMPONENT_VAR}_PACKAGE_DEPENDS "python3 (>= 3.10)"
    CACHE STRING "${PROJECT_NAME} deb package dependencies")

#
# PIP packaging.
#

if(KNP_PYTHON_BUILD_WHEEL)
    #message(FATAL_ERROR ${Python3_EXECUTABLE})
    add_custom_command(
        OUTPUT "${KNP_PYTHON_OUTPUT_DIRECTORY}/dist/knp-${PROJECT_VERSION}-py3-none-any.whl"
        COMMAND "${Python3_EXECUTABLE}" -m build --wheel
        WORKING_DIRECTORY "${KNP_PYTHON_OUTPUT_DIRECTORY}"
    )

    # TODO: automatically get all targets in CMakeLists.txt.
    add_custom_target(
        "${PROJECT_NAME}-pip-build-package" ALL
        DEPENDS
            "${PROJECT_NAME}_base_framework"
            "${PROJECT_NAME}_core"
            "${PROJECT_NAME}_neuron_traits"
            "${PROJECT_NAME}_synapse_traits"
            "${PROJECT_NAME}-copy-python-tests"
            "${PROJECT_NAME}-copy-common-files"
            "${PROJECT_NAME}-copy-common-python-files"
            "${PROJECT_NAME}-copy-common-project-files"
            "${KNP_PYTHON_OUTPUT_DIRECTORY}/dist/knp-${PROJECT_VERSION}-py3-none-any.whl"
    )
endif()
