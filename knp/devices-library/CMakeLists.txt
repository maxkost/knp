cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-devices VERSION "${KNP_VERSION}" LANGUAGES C CXX)

include(GNUInstallDirs)
include(clang-tidy)

source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers REGULAR_EXPRESSION "include/.*")

add_library("${PROJECT_NAME}-cpu"
    impl/cpu.cpp
)
add_library(KNP::Devices::CPU ALIAS "${PROJECT_NAME}-cpu")

target_include_directories("${PROJECT_NAME}-cpu" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries("${PROJECT_NAME}-cpu" PUBLIC knp-core)

#add_clang_tidy("${PROJECT_NAME}" CONFIG_FILE_PATH "${CMAKE_TIDY_CONFIG}" EXTRA_ARGS "--use-color")

if(KNP_ENABLE_COVERAGE)
    include(codecoverage)
    append_coverage_compiler_flags()
    link_target_with_gcov("${PROJECT_NAME}-cpu")
endif()

install(TARGETS "${PROJECT_NAME}-cpu"
        EXPORT "${PROJECT_NAME}-cpu"
#        NAMESPACE KNP::
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
