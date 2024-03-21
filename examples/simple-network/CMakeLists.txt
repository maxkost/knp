cmake_minimum_required(VERSION 3.25)
project(simple-network)

set(CMAKE_CXX_STANDARD 17)

add_executable("${PROJECT_NAME}" main.cpp)
target_link_libraries("${PROJECT_NAME}" PRIVATE KNP::BaseFramework::Core)

install(TARGETS "${PROJECT_NAME}"
        COMPONENT "examples")
