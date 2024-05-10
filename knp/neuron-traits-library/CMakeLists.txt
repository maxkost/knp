cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-neuron-traits VERSION "${KNP_VERSION}" LANGUAGES C CXX
        DESCRIPTION "Kaspersky Neuromorphic Platform neurons properties library"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

include(knp-functions)

find_package(Boost ${KNP_BOOST_MIN_VERSION} REQUIRED)

set(${PROJECT_NAME}_PUBLIC_INCLUDE_DIR "knp/neuron-traits")

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/*.h)

add_library("${PROJECT_NAME}"
        INTERFACE
        ${${PROJECT_NAME}_headers}
)

add_library(KNP::Neuron::Traits ALIAS "${PROJECT_NAME}")

target_include_directories("${PROJECT_NAME}" INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries("${PROJECT_NAME}" INTERFACE Boost::headers KNP::Meta)

install(TARGETS "${PROJECT_NAME}"
        EXPORT "${PROJECT_NAME}"
        COMPONENT "framework-dev")

install(DIRECTORY "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}"
        COMPONENT "framework-dev"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/knp"
        FILES_MATCHING PATTERN "*.h")
