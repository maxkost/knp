cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-cpu-backends-library VERSION "${KNP_VERSION}" LANGUAGES C CXX)

find_package(Boost 1.66.0 REQUIRED)

include(GNUInstallDirs)
include(clang-tidy)
include(knp-functions)

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/knp/backends/cpu-library/*.h)

knp_add_library("${PROJECT_NAME}"
    STATIC

    impl/init.cpp
    impl/delta_synapse_projection_impl.h
    impl/synaptic_resource_stdp.cpp

    ${${PROJECT_NAME}_headers}
    )
add_library(KNP::Backends::CPU::Library ALIAS "${PROJECT_NAME}")

source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers FILES ${${PROJECT_NAME}_headers} REGULAR_EXPRESSION "include/.*")
source_group(headers REGULAR_EXPRESSION "impl/.*")

target_include_directories("${PROJECT_NAME}" PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories("${PROJECT_NAME}" PRIVATE "impl")

target_link_libraries("${PROJECT_NAME}" PRIVATE Boost::headers spdlog::spdlog_header_only)
target_link_libraries("${PROJECT_NAME}" PUBLIC KNP::Core KNP::Devices::CPU)

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
