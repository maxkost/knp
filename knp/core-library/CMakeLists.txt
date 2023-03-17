cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-core VERSION "${KNP_VERSION}" LANGUAGES C CXX)

find_package(Boost 1.66.0 REQUIRED)
find_package(ZeroMQ REQUIRED)

include(GNUInstallDirs)
include(clang-tidy)
file(GLOB_RECURSE ${PROJECT_NAME}_headers include/knp/core/*.h)

knp_add_library("${PROJECT_NAME}"
    STATIC
    impl/backend.cpp
    impl/device.cpp
    impl/population.cpp
    impl/uid.cpp
    impl/projection.cpp
    impl/message_bus.cpp
    impl/message_endpoint.cpp

    ${${PROJECT_NAME}_headers}
    )
add_library(KNP::Core ALIAS "${PROJECT_NAME}")

source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers FILES ${${PROJECT_NAME}_headers} REGULAR_EXPRESSION "include/.*")

target_include_directories("${PROJECT_NAME}" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories("${PROJECT_NAME}" PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries("${PROJECT_NAME}" PRIVATE spdlog zmq)
target_link_libraries("${PROJECT_NAME}" PUBLIC knp-neuron-traits knp-synapse-traits)

# add_clang_tidy("${PROJECT_NAME}" CONFIG_FILE_PATH "${CMAKE_TIDY_CONFIG}" EXTRA_ARGS "--use-color")

if(KNP_ENABLE_COVERAGE)
    include(codecoverage)
    append_coverage_compiler_flags()
    link_target_with_gcov("${PROJECT_NAME}")
endif()

install(TARGETS "${PROJECT_NAME}"
        EXPORT "${PROJECT_NAME}"
#        NAMESPACE KNP::
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
