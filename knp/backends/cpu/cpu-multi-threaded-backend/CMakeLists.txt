cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-cpu-multi-threaded-backend VERSION "${KNP_VERSION}" LANGUAGES C CXX
        DESCRIPTION "Kaspersky Neuromorphic Platform CPU multi threading backend"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

set(${PROJECT_NAME}_PUBLIC_INCLUDE_DIR "knp/backends/cpu-multi-threaded")

include(GNUInstallDirs)
include(clang-tidy)
include(knp-functions)

find_package(Boost ${KNP_BOOST_MIN_VERSION} COMPONENTS system REQUIRED)

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/*.h)
source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers REGULAR_EXPRESSION "include/.*")

knp_add_library("${PROJECT_NAME}"
    BOTH
        impl/backend.cpp
        ${${PROJECT_NAME}_headers}
    ALIAS KNP::Backends::CPUMultiThreaded
    LINK_PRIVATE
        Boost::headers Boost::system spdlog::spdlog_header_only
        KNP::Backends::CPU::Library KNP::Backends::CPU::ThreadPool
    LINK_PUBLIC KNP::Core KNP::Devices::CPU
)

set_target_properties(
        "${PROJECT_NAME}"
        PROPERTIES
        VERSION "${CMAKE_PROJECT_VERSION}"
        SOVERSION "${CMAKE_PROJECT_VERSION_MAJOR}")

install(TARGETS "${PROJECT_NAME}"
        COMPONENT "cpu-multi-threaded-backend"
        LIBRARY
#        DESTINATION "${CMAKE_INSTALL_LIBDIR}/knp"
        )

install(TARGETS "${PROJECT_NAME}_static"
        COMPONENT "cpu-multi-threaded-backend-dev"
        ARCHIVE
#        DESTINATION "${CMAKE_INSTALL_LIBDIR}/knp"
        )

install(DIRECTORY "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}"
        COMPONENT "cpu-multi-threaded-backend-dev"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/knp/backends"
        FILES_MATCHING PATTERN "*.h")

set(CPACK_DEBIAN_CPU-MULTI-THREADED-BACKEND_DESCRIPTION "${PROJECT_DESCRIPTION}" PARENT_SCOPE)
set(CPACK_DEBIAN_CPU-MULTI-THREADED-BACKEND-DEV_DESCRIPTION "${PROJECT_DESCRIPTION} development files" PARENT_SCOPE)
