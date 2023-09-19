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
include(knp-functions)

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/knp/core/*.h)

set(${PROJECT_NAME}_FB_SOURCES
    impl/messaging/fbs/message_envelope.fbs
    impl/messaging/fbs/message_header.fbs
    impl/messaging/fbs/synapse_traits.fbs
    impl/messaging/fbs/spike_message.fbs
    impl/messaging/fbs/synaptic_impact_message.fbs
)

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
    impl/message_bus_zmq_impl/message_bus_impl.h
    impl/message_bus_zmq_impl/message_endpoint_impl.h
    impl/message_bus_zmq_impl/message_bus_impl.cpp
    impl/message_bus_zmq_impl/message_endpoint_impl.cpp
    impl/message_bus_impl.cpp
    impl/message_header.cpp
    impl/messaging/message_envelope.cpp
    impl/messaging/spike_message_impl.h
    impl/messaging/spike_message.cpp
    impl/messaging/synaptic_impact_message_impl.h
    impl/messaging/synaptic_impact_message.cpp
    impl/subscription.cpp

    ${${PROJECT_NAME}_headers}
    )
add_library(KNP::Core ALIAS "${PROJECT_NAME}")

source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers FILES ${${PROJECT_NAME}_headers} REGULAR_EXPRESSION "include/.*")

target_include_directories("${PROJECT_NAME}" PRIVATE ${Boost_INCLUDE_DIRS})

# Flatbuffers headers must be generated before core compilation started.
add_dependencies("${PROJECT_NAME}" "${PROJECT_NAME}_messaging")

target_link_libraries("${PROJECT_NAME}" PRIVATE spdlog::spdlog_header_only zmq flatbuffers "${PROJECT_NAME}_messaging")
# This is using in the library for message parameters.
target_link_libraries("${PROJECT_NAME}" PUBLIC KNP::Neuron::Traits KNP::Synapse::Traits)

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
