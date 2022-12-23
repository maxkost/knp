cmake_minimum_required(3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(knp VERSION "${KNP_VERSION}" LANGUAGES C CXX)

set(Boost_USE_STATIC_LIBS ON)


if(BUILD_TESTS)
    set(GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third-party/googletest")
#    set_target_properties(gtest PROPERTIES FOLDER third-party)
#    set_target_properties(gtest_main PROPERTIES FOLDER third-party)
#    set_target_properties(gmock PROPERTIES FOLDER third-party)
#    set_target_properties(gmock_main PROPERTIES FOLDER third-party)
    add_subdirectory("${GTEST_DIR}" third-party/googletest)
endif()
