cmake_minimum_required(VERSION 3.25)

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

if (TARGET hdf5-static)
    message(STATUS "Using static HDF5 library")
    set(HDF5_LIB hdf5-static)
elseif (TARGET HDF5::HDF5)
    message(STATUS "Using dynamic HDF5 library")
    set(HDF5_LIB HDF5::HDF5)
else()
    message(FATAL_ERROR "HDF5 library was not found")
endif()

knp_add_library("${PROJECT_NAME}-core"
    BOTH
    impl/backend_loader.cpp
    impl/storage/native/data_storage_common.cpp
    impl/storage/native/data_storage_json.cpp
    impl/storage/native/data_storage_hdf5.cpp
    impl/network.cpp
    impl/model.cpp
    impl/model_executor.cpp
    impl/input_converter.cpp
    impl/output_channel.cpp
    impl/sonata/save_network.cpp
    impl/sonata/load_network.cpp
    impl/sonata/csv_content.cpp
    impl/sonata/types/blifat_neuron.cpp
    impl/sonata/types/delta_synapse.cpp
    impl/sonata/types/resource_blifat_neuron.cpp
    impl/sonata/types/resource_delta_synapse.cpp
    impl/observer.cpp
    ${${PROJECT_NAME}_headers}
    ALIAS KNP::BaseFramework::Core
    LINK_PRIVATE
        spdlog::spdlog_header_only Boost::headers Boost::filesystem HighFive ${HDF5_LIB} simdjson::simdjson_static
    LINK_PUBLIC
        KNP::Core KNP::Neuron::Traits KNP::Synapse::Traits
    PRECOMP KNP::Core)

set_target_properties(
        "${PROJECT_NAME}-core"
        PROPERTIES
        VERSION "${CMAKE_PROJECT_VERSION}"
        SOVERSION "${CMAKE_PROJECT_VERSION_MAJOR}")

set(COMPONENT_NAME framework)

configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        "${CMAKE_INSTALL_LIBDIR}/cmake/")

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion)

install(TARGETS "${PROJECT_NAME}-core"
        EXPORT "${PROJECT_NAME}-core"
        LIBRARY
        COMPONENT "${COMPONENT_NAME}")
#install(EXPORT "${PROJECT_NAME}-core" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/cmake")

install(TARGETS "${PROJECT_NAME}-core_static"
        EXPORT "${PROJECT_NAME}-core_static"
        COMPONENT "${COMPONENT_NAME}-dev"
        ARCHIVE)

install(DIRECTORY "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}"
        COMPONENT "${COMPONENT_NAME}-dev"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/knp"
        FILES_MATCHING PATTERN "*.h")

install(FILES
        "${CMAKE_SOURCE_DIR}/LICENSE.txt"
        "${CMAKE_SOURCE_DIR}/README.md"
        COMPONENT "${COMPONENT_NAME}"
        TYPE DOC)

knp_packaging_set_parameters("${COMPONENT_NAME}" "${PROJECT_NAME}"
        DESCRIPTION "${PROJECT_DESCRIPTION}"
        DEPENDS backend)

set(CPACK_DEBIAN_${COMPONENT_VAR}_PACKAGE_DEPENDS "libboost-all (= ${Boost_VERSION}), libhdf5, libzmq5"
    CACHE STRING "${PROJECT_NAME} deb package dependencies")

knp_packaging_set_parameters("${COMPONENT_NAME}-dev" "${PROJECT_NAME}"
        DESCRIPTION "${PROJECT_DESCRIPTION} development files: C++ headers, CMake modules, static libraries"
        DEPENDS "${COMPONENT_NAME}")

set(CPACK_DEBIAN_${COMPONENT_VAR}-DEV_PACKAGE_DEPENDS "libboost-all-dev (= ${Boost_VERSION})"
    CACHE STRING "${PROJECT_NAME}-dev deb package dependencies")

set(CPACK_DEBIAN_${COMPONENT_VAR}-DEV_PACKAGE_RECOMMENDS "cppzmq-dev (>= 4.9.0-1), cppzmq (= 4.9.0-1)"
    CACHE STRING "${PROJECT_NAME}-dev deb package recommendations")

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
        COMPONENT "framework-dev"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
