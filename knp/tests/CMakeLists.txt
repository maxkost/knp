cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-tester VERSION "${KNP_VERSION}" LANGUAGES CXX
        DESCRIPTION "Kaspersky Neuromorphic Platform tester and tests")

if (NOT KNP_BUILD_TESTS)
    message(STATUS "Tests building disabled...")
    return()
endif()

include(CTest)

include(memcheck)

enable_testing()

include(GoogleTest)

file(GLOB_RECURSE COMMON_SOURCE CONFIGURE_DEPENDS
     "*.h"
     "*_test.cpp")

add_executable("${PROJECT_NAME}" ${COMMON_SOURCE} tester.cpp utility.cpp)

target_include_directories("${PROJECT_NAME}"
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/common"
    PRIVATE "${GTEST_DIR}/googletest/include"
    PRIVATE "${GTEST_DIR}/googlemock/include"
)

knp_get_hdf5_target(HDF5_LIB)

target_link_libraries("${PROJECT_NAME}" PRIVATE gtest gtest_main spdlog::spdlog HighFive ${HDF5_LIB})
target_link_libraries("${PROJECT_NAME}" PRIVATE KNP::BaseFramework::Core KNP::Backends::CPUSingleThreaded KNP::Backends::CPUMultiThreaded
        KNP::Devices::CPUStatic)
target_link_libraries("${PROJECT_NAME}" PRIVATE KNP::Backends::CPU::ThreadPool)

add_dependencies("${PROJECT_NAME}" knp-base-framework-core_static)

if (KNP_ENABLE_COVERAGE)
    include(codecoverage)
    append_coverage_compiler_flags()
    link_target_with_gcov("${PROJECT_NAME}")
endif()

if (${BUILD_GMOCK})
    target_link_libraries("${PROJECT_NAME}" PRIVATE gmock gmock_main)
endif()

gtest_discover_tests("${PROJECT_NAME}"
    # Set a working directory to find test data via paths relative to the project root
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

if (KNP_ENABLE_COVERAGE)
    message(STATUS "Coverage enabled")

    set(COV_EXECUTABLE "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    set(COV_NAME "${PROJECT_NAME}_coverage")
    set(COV_DEPENDENCIES "${PROJECT_NAME}")
    set(COV_BASE_DIRECTORY "${CMAKE_SOURCE_DIR}")

    set(CODE_COVERAGE_VERBOSE true)

    if (GCOVR_PATH)
        setup_target_for_coverage_gcovr_xml(
            NAME "${COV_NAME}"
            EXECUTABLE ${COV_EXECUTABLE}
            DEPENDENCIES ${COV_DEPENDENCIES}
            BASE_DIRECTORY "${COV_BASE_DIRECTORY}"
        )
        message(VERBOSE "Gcovr selected")
    elseif (LCOV_PATH)
        setup_target_for_coverage_lcov(
            NAME "${COV_NAME}"
            EXECUTABLE ${COV_EXECUTABLE}
            DEPENDENCIES ${COV_DEPENDENCIES}
            BASE_DIRECTORY "${COV_BASE_DIRECTORY}"
        )
        message(VERBOSE "Lcov selected")
    elseif (GCOV_PATH)
        setup_target_for_coverage_gcov_json(
            NAME "${COV_NAME}"
            EXECUTABLE ${COV_EXECUTABLE}
            DEPENDENCIES ${COV_DEPENDENCIES}
            BASE_DIRECTORY "${COV_BASE_DIRECTORY}"
        )
        message(STATUS "Gcov selected")
    else()
        message(WARNING "Coverage can't be enabled!")
    endif()
endif()

#[[
add_custom_target(test_memcheck
    COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --test-action memcheck
        --rerun-failed --output-on-failure
    COMMAND cat "${CMAKE_BINARY_DIR}/Testing/Temporary/MemoryChecker.*.log"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data")
]]
