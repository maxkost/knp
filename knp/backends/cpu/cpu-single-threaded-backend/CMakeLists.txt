cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-cpu-single-threaded-backend VERSION "${KNP_VERSION}" LANGUAGES C CXX)

set(${PROJECT_NAME}_PUBLIC_INCLUDE_DIR "knp/backends/cpu-single-threaded")

include(GNUInstallDirs)
include(clang-tidy)
include(knp-functions)

find_package(Boost ${KNP_BOOST_MIN_VERSION} REQUIRED)

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/*.h)
source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers REGULAR_EXPRESSION "include/.*")

knp_add_library("${PROJECT_NAME}"
    BOTH
        impl/backend.cpp
        ${${PROJECT_NAME}_headers}
    ALIAS KNP::Backends::CPUSingleThreaded
    LINK_PRIVATE
        Boost::headers spdlog::spdlog_header_only KNP::Backends::CPU::Library
    LINK_PUBLIC KNP::Core KNP::Devices::CPU
)

install(TARGETS "${PROJECT_NAME}" "${PROJECT_NAME}_static"
        EXPORT "${PROJECT_NAME}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/knp")

install(
        DIRECTORY "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/knp/backends"
        FILES_MATCHING PATTERN "*.h")
