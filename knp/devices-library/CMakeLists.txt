cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Required for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Suppress Boost warning.
cmake_policy(SET CMP0167 OLD)

project(knp-devices VERSION "${KNP_VERSION}" LANGUAGES C CXX
        DESCRIPTION "Kaspersky Neuromorphic Platform device parameters library"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

include(GNUInstallDirs)
include(clang-tidy)
include(knp-functions)

set(${PROJECT_NAME}_PUBLIC_INCLUDE_DIR "knp/devices")

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/*.h)

source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers REGULAR_EXPRESSION "include/.*")

knp_add_library("${PROJECT_NAME}-cpu"
    BOTH
    impl/cpu.cpp impl/cpu_power.cpp impl/cpu_power.h

    ${${PROJECT_NAME}_headers}

    LINK_PRIVATE
        spdlog::spdlog PCM_STATIC
        KNP::Core
    ALIAS KNP::Devices::CPU
)

set_target_properties(
        "${PROJECT_NAME}-cpu"
        PROPERTIES
        VERSION "${CMAKE_PROJECT_VERSION}"
        SOVERSION "${CMAKE_PROJECT_VERSION_MAJOR}")

#add_clang_tidy("${PROJECT_NAME}" CONFIG_FILE_PATH "${CMAKE_TIDY_CONFIG}" EXTRA_ARGS "--use-color")

if(KNP_ENABLE_COVERAGE)
    include(codecoverage)
    append_coverage_compiler_flags()
    link_target_with_gcov("${PROJECT_NAME}-cpu")
endif()


install(TARGETS "${PROJECT_NAME}-cpu"
        COMPONENT "framework-dev"
        EXPORT "${PROJECT_NAME}-cpu"
        LIBRARY
#        DESTINATION "${CMAKE_INSTALL_LIBDIR}/knp"
)

install(TARGETS "${PROJECT_NAME}-cpu_static"
        EXPORT "${PROJECT_NAME}-cpu_static"
        COMPONENT "framework-dev"
        ARCHIVE)

install(DIRECTORY "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}"
        COMPONENT "framework-dev"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/knp"
        FILES_MATCHING PATTERN "*.h")
