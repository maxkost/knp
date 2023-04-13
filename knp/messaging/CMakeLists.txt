cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-messaging VERSION "${KNP_VERSION}" LANGUAGES C CXX)

include(knp-functions)

find_package(flatbuffers REQUIRED)

set(${PROJECT_NAME}_SOURCES
    fbs/message_envelope.fbs
    fbs/message_header.fbs
    fbs/synapse_traits.fbs
    fbs/spike_message.fbs
    fbs/synaptic_impact_message.fbs
)

flatbuffers_generate_headers(
    TARGET "${PROJECT_NAME}"
    INCLUDE_PREFIX "knp_gen_headers"
    SCHEMAS ${${PROJECT_NAME}_SOURCES}
    # BINARY_SCHEMAS_DIR "${CMAKE_BINARY_DIR}/gen_includes"
    # FLAGS --gen-object-api
)

add_library(KNP::Messaging ALIAS "${PROJECT_NAME}")
