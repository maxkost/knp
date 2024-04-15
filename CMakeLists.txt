cmake_minimum_required(VERSION 3.25)

include_guard(GLOBAL)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(knp-functions)
include(third-party)
include(version)

get_knp_version()

if(NOT KNP_VERSION)
    message(FATAL_ERROR "KNP version mandatory for build, but can't be determined!")
endif()

project(knp VERSION "${KNP_VERSION}"
        DESCRIPTION "Kaspersky Neuromorphic Platform"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

set(KNP_BOOST_MIN_VERSION "1.74.0")

#
# KNP build options set.
#

# Build not from source directory, possible via add_subdirectory().
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(KNP_BUILD_TESTS_DEFAULT OFF)
    set(KNP_BUILD_DOCUMENTATION_DEFAULT OFF)
    set(KNP_BUILD_EXAMPLES_DEFAULT OFF)
else()
    set(KNP_BUILD_TESTS_DEFAULT ON)
    set(KNP_BUILD_DOCUMENTATION_DEFAULT ON)
    set(KNP_BUILD_EXAMPLES_DEFAULT ON)
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        set(KNP_ENABLE_AVX_DEFAULT ON)
    endif()
else()
    set(KNP_ENABLE_AVX_DEFAULT OFF)
endif()

if(KNP_MAINTAINER_BUILD)
    set(KNP_COVERAGE_DEFAULT OFF)
else()
    set(KNP_COVERAGE_DEFAULT ON)
endif()

option(KNP_BUILD_TESTS "Build tests" ${KNP_BUILD_TESTS_DEFAULT})
option(KNP_BUILD_DOCUMENTATION "Build doxygen auto documentation" ${KNP_BUILD_DOCUMENTATION_DEFAULT})
option(KNP_BUILD_EXAMPLES "Build usage examples" ${KNP_BUILD_EXAMPLES_DEFAULT})
option(KNP_MAINTAINER_BUILD "Build for maintainer, but not for the development purposes" OFF)
option(KNP_ENABLE_COVERAGE "Enable coverage checking" ${KNP_COVERAGE_DEFAULT})
option(KNP_ENABLE_AVX "Enable AVX and other CPU-specific extensions in the release build" ${KNP_ENABLE_AVX_DEFAULT})

if (KNP_ENABLE_AVX)
    message(STATUS "AVX enabled.")
    message(STATUS "Setting release optimization flags...")
    # -pipe
    set(CMAKE_C_FLAGS_RELEASE "-march=native -mtune=native -mno-vzeroupper -Ofast -funroll-loops -fomit-frame-pointer -finline")

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fopt-info-vec-optimized")
    endif()

    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
    add_definitions(-include x86intrin.h)
    # -include bits/stdc++.h
endif()

set(ENABLE_FAST_MATH ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT KNP_MAINTAINER_BUILD)
    execute_process(COMMAND pre-commit install
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    if(ret AND NOT ret EQUAL 0)
        message(FATAL_ERROR "Can't install pre-commit hooks!")
    endif()
    execute_process(COMMAND pre-commit install -t commit-msg
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

if(KNP_BUILD_TESTS)
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=0)
endif()

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(print-variables)


#
# Build third-party.
#

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/third-party)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/third-party)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/third-party)

# Make cache variables for install destinations.
include(GNUInstallDirs)

include(print-variables)

set(SPDLOG_BUILD_PIC ON)
set(EXTLIB_FROM_SUBMODULES ON)
set(SONATA_TESTS OFF)
set(SONATA_VERSION 1.0)
add_third_party(libsonata)
add_third_party(csv2)
file(GLOB THIRD_PARTY_INCLUDES LIST_DIRECTORIES true "third-party/**/include")
include_directories(third-party ${THIRD_PARTY_INCLUDES})

include(print-variables)

set(FLATBUFFERS_BUILD_TESTS OFF)
set(FLATBUFFERS_BUILD_CPP17 ON)
add_third_party(flatbuffers)
add_third_party(simdjson)

add_third_party(pcm)

set(SPDLOG_BUILD_PIC ON)
add_third_party(spdlog)

if(KNP_BUILD_TESTS)
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=0)
    add_third_party(googletest)
endif()


#
# Build KNP.
#

file(GLOB THIRD_PARTY_INCLUDES LIST_DIRECTORIES true "third-party/**/include")
include_directories(third-party ${THIRD_PARTY_INCLUDES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_TIDY_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/knp/.clang-tidy" CACHE PATH  "Clang-tidy config file")

add_subdirectory(knp)

if (KNP_BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif()

if(KNP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()


#
# Packaging.
#

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}" CACHE STRING "The resulting package name")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_ICON "doc/html/img/neuromorphic-platform.svg")

set(CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})

set(CPACK_PACKAGE_VENDOR "Kaspersky Laboratory")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Artiom N.")
set(CPACK_PACKAGE_CONTACT "my@E-MAIL.net")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CMAKE_PROJECT_DESCRIPTION}"
        CACHE STRING "Package description for the package metadata")
set(CPACK_PACKAGE_DESCRIPTION "Kaspersky Neuromorphic Platform - spiking networks simulator")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/_packages")

# https://unix.stackexchange.com/a/11552/254512
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_INSTALL_PREFIX}")
set(CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE)

# On Linux grouping must be ignored: one component - one package.
set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)


#
# Deb package.
#

set(CPACK_DEB_PACKAGE_COMPONENT ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)

set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")

set(CPACK_DEBIAN_PACKAGE_SECTION "science")

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

#set(CPACK_DEBIAN_PACKAGE_NAME ${PROJECT_NAME}-dev)
#set(CPACK_DEBIAN_FILE_NAME "${PROJECT_NAME}-dev_${PROJECT_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS OFF)


#
# RPM package.
#

set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_PACKAGE_RELOCATABLE  ON)
set(CPACK_RPM_COMPONENT_INSTALL ON)

set(CPACK_RPM_PACKAGE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RPM_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
set(CPACK_RPM_PACKAGE_GROUP "science")


include (CPack)
