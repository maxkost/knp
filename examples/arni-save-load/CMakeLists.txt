cmake_minimum_required(VERSION 3.25)
project(arni-save-load)

set(CMAKE_CXX_STANDARD 17)

add_executable(arni-save-load main.cpp)
target_link_libraries("${PROJECT_NAME}" PRIVATE KNP::BaseFramework::Core)
