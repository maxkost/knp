cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-base-framework VERSION "${KNP_VERSION}" LANGUAGES C CXX
        DESCRIPTION "Kaspersky Neuromorphic Platform C++ framework"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

set(${PROJECT_NAME}_PUBLIC_INCLUDE_DIR "knp/framework")

find_package(Boost ${KNP_BOOST_MIN_VERSION} COMPONENTS filesystem REQUIRED)

include(GNUInstallDirs)
include(knp-functions)

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/*.h)

source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers FILES ${${PROJECT_NAME}_headers} REGULAR_EXPRESSION "include/.*")

knp_add_library("${PROJECT_NAME}-core"
    BOTH
    impl/backend_loader.cpp
    impl/network.cpp
    impl/model.cpp
    impl/model_executor.cpp
    impl/input_converter.cpp
    impl/output_channel.cpp
    impl/observer.cpp

    ${${PROJECT_NAME}_headers}

    ALIAS KNP::BaseFramework::Core
    LINK_PRIVATE
        spdlog::spdlog_header_only Boost::headers Boost::filesystem
    LINK_PUBLIC
        KNP::Core KNP::Neuron::Traits KNP::Synapse::Traits)

set_target_properties(
        "${PROJECT_NAME}-core"
        PROPERTIES
        VERSION "${CMAKE_PROJECT_VERSION}"
        SOVERSION "${CMAKE_PROJECT_VERSION_MAJOR}")

install(TARGETS "${PROJECT_NAME}-core"
        LIBRARY
#        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/knp"
        COMPONENT "framework")
#install(EXPORT "${PROJECT_NAME}-core" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/cmake")

install(TARGETS "${PROJECT_NAME}-core_static"
        COMPONENT "framework-dev"
        ARCHIVE
#        DESTINATION "${CMAKE_INSTALL_LIBDIR}/knp"
        )

install(DIRECTORY "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}"
        COMPONENT "framework-dev"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/knp"
        FILES_MATCHING PATTERN "*.h")
