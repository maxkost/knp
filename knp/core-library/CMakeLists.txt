cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-core VERSION "${KNP_VERSION}" LANGUAGES C CXX)

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
    )
add_library(KNP::Core ALIAS "${PROJECT_NAME}")

source_group(source REGULAR_EXPRESSION "impl/.*")

target_include_directories("${PROJECT_NAME}" PRIVATE ${Boost_INCLUDE_DIRS} "impl")

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
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION
    "${CMAKE_INSTALL_LIBDIR}/cmake/"
)

install(TARGETS "${PROJECT_NAME}"
    EXPORT "${PROJECT_NAME}"
    COMPONENT "${PROJECT_NAME}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/knp")

install(
    FILES
        ${${PROJECT_NAME}_headers}
        "${CMAKE_CURRENT_BINARY_DIR}/include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/version.h"
    DESTINATION "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}")

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)


#
# Packaging.
#

set(CPACK_PACKAGE_CONTACT "Artiom N. <>")
set(CPACK_PACKAGE_NAME    "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
#SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/packages")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Kaspersky Neuromorphic Platform core")
set(CPACK_PACKAGE_DESCRIPTION "\
    Kaspersky Neuromorphic Platform is a spiking networks simulator\n\
    Core library for development.")

set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX})
set(CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

#
# Deb package.
#

set(CPACK_DEBIAN_PACKAGE_SECTION "science")

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev (>= ${KNP_BOOST_MIN_VERSION}),")

set(CPACK_DEBIAN_PACKAGE_NAME ${PROJECT_NAME}-dev)

set(CPACK_DEBIAN_FILE_NAME "${PROJECT_NAME}-dev_${PROJECT_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Artiom N.")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "cppzmq-dev (>= 4.9.0-1) , cppzmq (4.9.0-1)")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)

set(CPACK_DEB_PACKAGE_COMPONENT ON)

set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_RPM_COMPONENT_INSTALL ON)

#
# RPM package.
#

set(CPACK_RPM_PACKAGE_RELOCATABLE  ON)
set(CPACK_RPM_PACKAGE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")

include (CPack)
