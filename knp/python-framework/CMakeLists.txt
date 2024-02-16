cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp_python_framework LANGUAGES C CXX)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(Boost 1.66.0 COMPONENTS python REQUIRED)

include(knp-functions)

file(GLOB_RECURSE MODULES_SOURCE CONFIGURE_DEPENDS
     "impl/core/*.h"
     "impl/core/*.cpp")

knp_add_library("${PROJECT_NAME}"
    SHARED
    ${MODULES_SOURCE}
)

target_include_directories("${PROJECT_NAME}" PRIVATE ${Python_INCLUDE_DIRS})
target_link_libraries("${PROJECT_NAME}" PRIVATE KNP::Core KNP::BaseFramework::Core Boost::headers Boost::python)
