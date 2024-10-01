cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Required for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-cpu-single-threaded-backend VERSION "${KNP_VERSION}" LANGUAGES C CXX
        DESCRIPTION "Kaspersky Neuromorphic Platform single-threaded CPU backend"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

set(${PROJECT_NAME}_PUBLIC_INCLUDE_DIR "knp/backends/cpu-single-threaded")

include(GNUInstallDirs)
include(clang-tidy)
include(knp-functions)

find_package(Boost ${KNP_BOOST_MIN_VERSION} COMPONENTS system filesystem REQUIRED CONFIG)

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/*.h)
source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers REGULAR_EXPRESSION "include/.*")

knp_add_library("${PROJECT_NAME}"
    BOTH
        impl/backend.cpp
        impl/get_network.cpp
        ${${PROJECT_NAME}_headers}
    ALIAS KNP::Backends::CPUSingleThreaded
    LINK_PRIVATE
        Boost::headers ${Boost_LIBRARIES} spdlog::spdlog KNP::Backends::CPU::Library
    LINK_PUBLIC
        KNP::Core KNP::Devices::CPUStatic
)

set_target_properties(
        "${PROJECT_NAME}"
        PROPERTIES
        VERSION "${CMAKE_PROJECT_VERSION}"
        SOVERSION "${CMAKE_PROJECT_VERSION_MAJOR}")

knp_get_component_name("${PROJECT_NAME}" COMPONENT_NAME)
knp_get_component_var_name("${COMPONENT_NAME}" COMPONENT_VAR)

install(TARGETS "${PROJECT_NAME}"
        EXPORT "${PROJECT_NAME}"
        COMPONENT "${COMPONENT_NAME}"
        LIBRARY)

install(TARGETS "${PROJECT_NAME}_static"
        EXPORT "${PROJECT_NAME}_static"
        COMPONENT "${COMPONENT_NAME}-dev"
        ARCHIVE)

install(DIRECTORY "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}"
        COMPONENT "${COMPONENT_NAME}-dev"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/knp/backends"
        FILES_MATCHING PATTERN "*.h")

knp_packaging_set_parameters("${COMPONENT_NAME}" "${PROJECT_NAME}"
    DESCRIPTION "${PROJECT_DESCRIPTION}")

set(CPACK_DEBIAN_${COMPONENT_VAR}_PACKAGE_DEPENDS "libboost-all (= ${Boost_VERSION}), libzmq" CACHE STRING "${PROJECT_NAME} deb package dependencies")

knp_packaging_set_parameters("${COMPONENT_NAME}-dev" "${PROJECT_NAME}"
    DESCRIPTION "${PROJECT_DESCRIPTION} development package: C++ headers and CMake modules"
    DEPENDS "${COMPONENT_NAME}"
)

set(CPACK_DEBIAN_${COMPONENT_VAR}_PACKAGE_PROVIDES "knp-backend" CACHE STRING "${PROJECT_NAME} provides")
