cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-core VERSION "${KNP_VERSION}" LANGUAGES C CXX
        DESCRIPTION "Kaspersky Neuromorphic Platform core"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

find_package(Boost ${KNP_BOOST_MIN_VERSION} REQUIRED)
find_package(ZeroMQ REQUIRED)

include(GNUInstallDirs)
# Need for installation.
include(CMakePackageConfigHelpers)
include(clang-tidy)
include(knp-functions)

set(${PROJECT_NAME}_PUBLIC_INCLUDE_DIR "knp/core")

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/*.h)

set(${PROJECT_NAME}_FB_SOURCES
    impl/messaging/fbs/message_envelope.fbs
    impl/messaging/fbs/message_header.fbs
    impl/messaging/fbs/synapse_traits.fbs
    impl/messaging/fbs/spike_message.fbs
    impl/messaging/fbs/synaptic_impact_message.fbs
)


#
# Build.
#

flatbuffers_generate_headers(
    TARGET "${PROJECT_NAME}_messaging"
    INCLUDE_PREFIX "knp_gen_headers"
    SCHEMAS ${${PROJECT_NAME}_FB_SOURCES}
    # BINARY_SCHEMAS_DIR "${CMAKE_BINARY_DIR}/gen_includes"
    # FLAGS --gen-object-api
)

knp_add_library("${PROJECT_NAME}"
    STATIC
    impl/backend.cpp
    impl/device.cpp
    impl/population.cpp
    impl/uid.cpp
    impl/projection.cpp
    impl/message_bus.cpp
    impl/message_endpoint.cpp
    impl/message_bus_zmq_impl/message_bus_zmq_impl.h
    impl/message_bus_zmq_impl/message_endpoint_zmq_impl.h
    impl/message_bus_zmq_impl/message_bus_zmq_impl.cpp
    impl/message_bus_zmq_impl/message_endpoint_zmq_impl.cpp
    impl/message_bus_cpu_impl/message_bus_cpu_impl.cpp
    impl/message_bus_cpu_impl/message_bus_cpu_impl.h
    impl/message_bus_cpu_impl/message_endpoint_cpu_impl.h
    impl/message_bus_impl.h
    impl/message_header.cpp
    impl/messaging/message_envelope.cpp
    impl/messaging/spike_message_impl.h
    impl/messaging/spike_message.cpp
    impl/messaging/synaptic_impact_message_impl.h
    impl/messaging/synaptic_impact_message.cpp
    impl/subscription.cpp

    ${${PROJECT_NAME}_headers}

    LINK_PRIVATE
        spdlog::spdlog_header_only zmq flatbuffers "${PROJECT_NAME}_messaging"
    LINK_PUBLIC
        # This is using in the library for message parameters.
        KNP::Neuron::Traits KNP::Synapse::Traits
    ALIAS KNP::Core
)

source_group(source REGULAR_EXPRESSION "impl/.*")

target_include_directories("${PROJECT_NAME}" PRIVATE ${Boost_INCLUDE_DIRS} "impl")

# Flatbuffers headers must be generated before core compilation started.
add_dependencies("${PROJECT_NAME}" "${PROJECT_NAME}_messaging")

# add_clang_tidy("${PROJECT_NAME}" CONFIG_FILE_PATH "${CMAKE_TIDY_CONFIG}" EXTRA_ARGS "--use-color")

if(KNP_ENABLE_COVERAGE)
    include(codecoverage)
    append_coverage_compiler_flags()
    link_target_with_gcov("${PROJECT_NAME}")
endif()


#
# Installation.
#

message(STATUS "Installation config...")

set(PACKAGE_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR})

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/version.h")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION
    "${CMAKE_INSTALL_LIBDIR}/cmake/")

install(TARGETS "${PROJECT_NAME}"
        ARCHIVE
        COMPONENT "framework-dev"
#        DESTINATION "${CMAKE_INSTALL_LIBDIR}/knp"
        )

install(DIRECTORY "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}"
        COMPONENT "framework-dev"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/knp"
        FILES_MATCHING PATTERN "*.h")

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/version.h"
        COMPONENT "framework-dev"
        DESTINATION "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}")

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
        COMPONENT "framework-dev"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
