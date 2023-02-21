cmake_minimum_required(VERSION 3.22) # TODO: increase to 3.25

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include("print_variables")
include("third-party")
include("version")


project(knp)

option(KNP_BUILD_TESTS "Build tests" ON)
option(KNP_ENABLE_COVERAGE "Enable coverage test" ON)
option(KNP_BUILD_DOCUMENTATION "Build doxygen auto documentation" ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_TIDY_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/knp/.clang-tidy" CACHE PATH  "Clang-tidy config file")
set(DOXYFILE_IN "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")

# Make cache variables for install destinations.
include(GNUInstallDirs)

file(GLOB THIRD_PARTY_INCLUDES LIST_DIRECTORIES true "third-party/**/include")
include_directories(third-party ${THIRD_PARTY_INCLUDES})

add_third_party(googletest)
add_third_party(spdlog)

set(CPPZMQ_BUILD_TESTS off)
add_third_party(cppzmq)
add_third_party(pybind11)
add_third_party(pcm)

add_subdirectory(knp)

if(KNP_BUILD_DOCUMENTATION)
    include("doxygen")
    foreach(_i
            core-library base-framework
            neuron-traits-library synapse-traits-library
            devices-library)
        list(APPEND _strip_list "${CMAKE_CURRENT_SOURCE_DIR}/knp/${_i}/include")
    endforeach()

    add_doxygen(knp-doc "${CMAKE_CURRENT_SOURCE_DIR}/knp/autodoc/Doxyfile.in"
        PROJECT_VERSION "0.0.1"
        PROJECT_BRIEF "Spiking Networks Platform"
        OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc_doxygen"
        INPUT "${CMAKE_CURRENT_SOURCE_DIR}/knp"
        EXAMPLE_PATH "knp/tests"
        PROJECT_LOGO "${CMAKE_CURRENT_SOURCE_DIR}/knp/autodoc/logo.png"
        SOURCE_BROWSER YES
        BUNDLE_ID "com.kaspersky.KNP"
        PUBLISHER_NAME "Kaspersky Lab"
        EXCLUDE_PATTERNS "knp/tests;knp/.descr;knp/autodoc;.*"
        STRIP_FROM_INC_PATH "${_strip_list}"
    )
    # Automatically build doc.
    add_dependencies(knp-core knp-doc)
endif()
