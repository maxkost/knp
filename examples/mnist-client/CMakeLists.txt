cmake_minimum_required(VERSION 3.22)
project(MnistExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(OpenCV 4.0.0 REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED )
include_directories (${OpenCV_INCLUDE_DIRS})

if (NOT TARGET KNP::BaseFramework::Core)
    find_package(knp-base-framework REQUIRED)
endif()

add_executable(MnistExample main.cpp visualize_network.cpp graph_physics.cpp)
target_link_libraries(MnistExample PRIVATE KNP::BaseFramework::Core ${OpenCV_LIBS} ${Boost_LIBRARIES})
