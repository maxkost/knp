cmake_minimum_required(VERSION 3.25)
project(save-load-networks)

set(CMAKE_CXX_STANDARD 17)

add_executable("${PROJECT_NAME}" main.cpp)
set(EXTLIB_FROM_SUBMODULES ON)
target_link_libraries("${PROJECT_NAME}" PRIVATE KNP::BaseFramework::Core)
