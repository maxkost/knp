cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-messaging VERSION "${KNP_VERSION}" LANGUAGES C CXX)

include(knp-functions)

find_package(flatbuffers)

build_flatbuffers("spike_message.fbs;synaptic_impact_message.fbs"
                  "${CMAKE_CURRENT_LIST_DIR}"
                  "${PROJECT_NAME}"
                  ""
                  "${CMAKE_BINARY_DIR}"
                  ""
                  "")
