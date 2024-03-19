cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-base-framework VERSION "${KNP_VERSION}" LANGUAGES C CXX)

find_package(Boost ${KNP_BOOST_MIN_VERSION} COMPONENTS filesystem REQUIRED)

include(GNUInstallDirs)
include(knp-functions)

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/knp/framework/*.h)

source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers FILES ${${PROJECT_NAME}_headers} REGULAR_EXPRESSION "include/.*")

knp_add_library("${PROJECT_NAME}-core"
    SHARED
    impl/backend_loader.cpp
    impl/network.cpp
    impl/model.cpp
    impl/model_executor.cpp
    impl/input_converter.cpp
    impl/output_channel.cpp
    impl/observer.cpp

    ${${PROJECT_NAME}_headers})
add_library(KNP::BaseFramework::Core ALIAS "${PROJECT_NAME}-core")

target_link_libraries("${PROJECT_NAME}-core" PRIVATE spdlog::spdlog_header_only Boost::headers Boost::filesystem)
target_link_libraries("${PROJECT_NAME}-core" PUBLIC KNP::Core KNP::Neuron::Traits KNP::Synapse::Traits)

install(TARGETS "${PROJECT_NAME}-core"
        EXPORT "${PROJECT_NAME}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        PUBLIC_HEADER
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}"
        )
