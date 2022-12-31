cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(knp-tester CXX)

if(NOT BUILD_TESTS)
    message(STATUS "Tests building disabled...")
    return()
endif()

include(GoogleTest)
enable_testing()

file(GLOB COMMON_SOURCE common/*.cpp common/*.h)

add_executable("${PROJECT_NAME}" ${COMMON_SOURCE} tester.cpp)


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
#    target_include_directories("${PROJECT_NAME}-${BACKEND}"
#        PRIVATE "${GTEST_DIR}/googletest/include"
#        PRIVATE "${GTEST_DIR}/googlemock/include"
#    )
#
#    # target_compile_options("${PROJECT_NAME}-${BACKEND}" PRIVATE)
#    target_link_libraries("${PROJECT_NAME}-${BACKEND}" PRIVATE "snn_backend_${BACKEND}" gtest gtest_main)
#
#
#    gtest_discover_tests("${PROJECT_NAME}-${BACKEND}"
#        # Set a working directory to find test data via paths relative to the project root
#        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data"
#        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data"
#    )
#endforeach()

target_link_libraries("${PROJECT_NAME}" PRIVATE gtest gtest_main)

if(${BUILD_GMOCK})
    target_link_libraries("${PROJECT_NAME}" PRIVATE gmock gmock_main)
endif()
