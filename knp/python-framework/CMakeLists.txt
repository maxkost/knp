cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp_python_framework LANGUAGES C CXX)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(Boost 1.66.0 COMPONENTS python REQUIRED)

include(knp-functions)

knp_add_python_module(core LINK_LIBRARIES KNP::Core KNP::BaseFramework::Core)
