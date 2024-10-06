cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV)
if (NOT OpenCV_FOUND)
    message(WARNING "OpenCV not found, skipping MNIST example...")
    return()
endif()

include_directories(${OpenCV_INCLUDE_DIRS})

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.30")
    # Suppress Boost warning.
    cmake_policy(SET CMP0167 OLD)
endif()

find_package(Boost ${KNP_BOOST_MIN_VERSION} COMPONENTS program_options REQUIRED)

if (NOT TARGET KNP::BaseFramework::Core)
    find_package(knp-base-framework REQUIRED)
endif()

project(MnistExample)
add_executable(MnistExample
    main.cpp
    graph_physics.cpp
    inference.cpp
    process_data.cpp
    visualize_network.cpp
)

target_link_libraries(MnistExample PRIVATE KNP::BaseFramework::Core ${OpenCV_LIBS} ${Boost_LIBRARIES})
