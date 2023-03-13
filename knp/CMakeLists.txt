cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp VERSION "${KNP_VERSION}" LANGUAGES C CXX)

set(Boost_USE_STATIC_LIBS ON)

add_subdirectory(neuron-traits-library)
add_subdirectory(synapse-traits-library)
add_subdirectory(core-library)
add_subdirectory(devices-library)
add_subdirectory(backends)
add_subdirectory(base-framework)
add_subdirectory(python-framework)
add_subdirectory(autodoc)
add_subdirectory(tests)
