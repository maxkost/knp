cmake_minimum_required(VERSION 3.22)
project(save-load-networks)

set(CMAKE_CXX_STANDARD 17)

add_executable("${PROJECT_NAME}" main.cpp)
set(EXTLIB_FROM_SUBMODULES ON)
target_link_libraries("${PROJECT_NAME}" PRIVATE HighFive sonata_static KNP::BaseFramework::Core)
