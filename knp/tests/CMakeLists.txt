cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(knp-tester CXX)

option(KNP_ENABLE_COVERAGE "Coverage enabled" ON)


if(NOT KNP_BUILD_TESTS)
    message(STATUS "Tests building disabled...")
    return()
endif()

include(Dart)
include(CTest)

enable_testing()

include(GoogleTest)


file(GLOB_RECURSE COMMON_SOURCE CONFIGURE_DEPENDS
     "*.cpp"
     "*.h")

add_executable("${PROJECT_NAME}" ${COMMON_SOURCE})


#file(GLOB BACKENDS LIST_DIRECTORIES yes "${CMAKE_CURRENT_SOURCE_DIR}/../include/snn-library/backends/*")

#foreach(BACKEND ${BACKENDS})
#    get_filename_component(BACKEND "${BACKEND}" NAME)
#    file(GLOB TEST_BACKEND_SOURCE ${BACKEND}/*.cpp ${BACKEND}/*.h)
#
#    string(TOUPPER "${BACKEND}" BE_UPPER)
#
#    if(NOT "${BUILD_${BE_UPPER}_BACKEND}")
#        message(STATUS "Tests for the back-end \"${BE_UPPER}\" will not be built...")
#        continue()
#    endif()
#
#    message(STATUS "Tests for the back-end \"${BE_UPPER}\" will be built.")
#    add_executable("${PROJECT_NAME}-${BACKEND}" ${COMMON_SOURCE} ${TEST_BACKEND_SOURCE} ../include/snn-library/utilities/network_load.h cpu/network_file_test.cpp common/net_from_csv_test.cpp)
#
#
#    # target_compile_options("${PROJECT_NAME}-${BACKEND}" PRIVATE)
#    target_link_libraries("${PROJECT_NAME}-${BACKEND}" PRIVATE "snn_backend_${BACKEND}")
#
#
#endforeach()

target_include_directories("${PROJECT_NAME}"
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/common"
    PRIVATE "${GTEST_DIR}/googletest/include"
    PRIVATE "${GTEST_DIR}/googlemock/include"
)

target_link_libraries("${PROJECT_NAME}" PRIVATE gtest gtest_main spdlog)
target_link_libraries("${PROJECT_NAME}" PRIVATE KNP::Core KNP::BaseFramework::Core)

if(KNP_ENABLE_COVERAGE)
    include(codecoverage)
    append_coverage_compiler_flags()
    link_target_with_gcov("${PROJECT_NAME}")
endif()

if(${BUILD_GMOCK})
    target_link_libraries("${PROJECT_NAME}" PRIVATE gmock gmock_main)
endif()

gtest_discover_tests("${PROJECT_NAME}"
    # Set a working directory to find test data via paths relative to the project root
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)


if(KNP_ENABLE_COVERAGE)
    message(STATUS "Coverage enabled")

    set(COV_EXECUTABLE "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    set(COV_NAME "${PROJECT_NAME}_coverage")
    set(COV_DEPENDENCIES "${PROJECT_NAME}")
    set(COV_BASE_DIRECTORY "${CMAKE_SOURCE_DIR}")

    set(CODE_COVERAGE_VERBOSE true)

    if(GCOVR_PATH)
        setup_target_for_coverage_gcovr_xml(
            NAME "${COV_NAME}"
            EXECUTABLE ${COV_EXECUTABLE}
            DEPENDENCIES ${COV_DEPENDENCIES}
            BASE_DIRECTORY "${COV_BASE_DIRECTORY}"
        )
        message(VERBOSE "Gcovr selected")
    elseif(LCOV_PATH)
        setup_target_for_coverage_lcov(
            NAME "${COV_NAME}"
            EXECUTABLE ${COV_EXECUTABLE}
            DEPENDENCIES ${COV_DEPENDENCIES}
            BASE_DIRECTORY "${COV_BASE_DIRECTORY}"
        )
        message(VERBOSE "Lcov selected")
    elseif(GCOV_PATH)
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

add_custom_target(test_memcheck
    COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --test-action memcheck
        --rerun-failed --output-on-failure
    COMMAND cat "${CMAKE_BINARY_DIR}/Testing/Temporary/MemoryChecker.*.log"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data")
