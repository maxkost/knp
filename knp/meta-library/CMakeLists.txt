cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-meta VERSION "${KNP_VERSION}" LANGUAGES C CXX
        DESCRIPTION "Kaspersky Neuromorphic Platform meta functions and templates library"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

set(${PROJECT_NAME}_PUBLIC_INCLUDE_DIR "knp/meta")

find_package(Boost ${KNP_BOOST_MIN_VERSION} REQUIRED)
find_package(ZeroMQ REQUIRED)

include(GNUInstallDirs)
include(clang-tidy)
include(knp-functions)

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/*.h)
source_group(headers FILES ${${PROJECT_NAME}_headers} REGULAR_EXPRESSION "include/.*")

add_library("${PROJECT_NAME}"
    INTERFACE
    ${${PROJECT_NAME}_headers}
)

add_library(KNP::Meta ALIAS "${PROJECT_NAME}")

target_include_directories("${PROJECT_NAME}" INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories("${PROJECT_NAME}" INTERFACE ${Boost_INCLUDE_DIRS})

install(DIRECTORY "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}"
        COMPONENT "framework-dev"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/knp"
        FILES_MATCHING PATTERN "*.h")
