cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-base-framework VERSION "${KNP_VERSION}" LANGUAGES C CXX)


find_package(Boost 1.66.0 REQUIRED)

include(GNUInstallDirs)

source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers FILES ${${PROJECT_NAME}_headers} REGULAR_EXPRESSION "include/.*")

add_library("${PROJECT_NAME}-core" impl/backend_loader.cpp)
add_library(KNP::Framework::Core ALIAS "${PROJECT_NAME}-core")

target_include_directories("${PROJECT_NAME}-core" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries("${PROJECT_NAME}-core" PUBLIC spdlog::spdlog_header_only knp-core)
