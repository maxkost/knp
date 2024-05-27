cmake_minimum_required(VERSION 3.25)

include_guard(GLOBAL)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CMakeDependentOption)
include(third-party)
include(version)

knp_get_version()

if(NOT KNP_VERSION)
    message(FATAL_ERROR "KNP version mandatory for build, but can't be determined!")
endif()

enable_language(CXX)

if (POLICY CMP0091)
    # Enable MSVS library select, using property.
    cmake_policy(SET CMP0091 NEW)
endif()

project(knp VERSION "${KNP_VERSION}"
        DESCRIPTION "Kaspersky Neuromorphic Platform"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

set(KNP_BOOST_MIN_VERSION "1.74.0")
set(KNP_ROOT_DIR "${PROJECT_SOURCE_DIR}" CACHE STRING "KNP root directory")

foreach (boost_ver RANGE 83 89)
    list(APPEND Boost_ADDITIONAL_VERSIONS "1.${boost_ver}")
endforeach()

# WARNING: Don't change variables case!
set(Boost_USE_MULTITHREADED ON)

set(Boost_ALL_DYN_LINK OFF)

if (NOT Boost_ROOT)
    if (DEFINED ENV{Boost_ROOT})
        set(Boost_ROOT "$ENV{Boost_ROOT}")
    elseif(DEFINED ENV{BOOST_ROOT})
        set(Boost_ROOT "$ENV{BOOST_ROOT}")
    endif()
endif()

if (Boost_ROOT)  # WIN32?
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_RUNTIME ON)
    set(Boost_USE_STATIC_RUNTIME ON)
    # Boost detection setup.
    if(NOT BOOST_INCLUDEDIR)
        set(BOOST_INCLUDEDIR "${BOOST_ROOT}")
    endif()

    if (NOT BOOST_LIBRARYDIR)
        if(EXISTS "${Boost_ROOT}/stage/lib")
            set(BOOST_LIBRARYDIR "${Boost_ROOT}/stage/lib")
        elseif(EXISTS "${Boost_ROOT}/stage")
            set(BOOST_LIBRARYDIR "${Boost_ROOT}/stage")
        else()
            file(GLOB _BOOST_LIB_DIRS LIST_DIRECTORIES true "${Boost_ROOT}/lib*-*-*")
            list(GET _BOOST_LIB_DIRS 0 BOOST_LIBRARYDIR)
            # set(Boost_LIB_PREFIX "")
        endif()
    endif()

    message(STATUS "Boost library dir: ${BOOST_LIBRARYDIR}")
endif()

# set(Boost_DEBUG ON)

if (MSVC)
    find_package(Boost ${KNP_BOOST_MIN_VERSION})
    add_link_options("/LIBPATH:${Boost_LIBRARY_DIR_RELEASE}")
endif()


#
# KNP build options set.
#

# Build not from source directory, possible via add_subdirectory().
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(KNP_BUILD_AUTONOMOUS FALSE)
else()
    set(KNP_BUILD_AUTONOMOUS TRUE)
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        set(KNP_ENABLE_AVX_DEFAULT ON)
    endif()
else()
    set(KNP_ENABLE_AVX_DEFAULT OFF)
endif()

if (KNP_MAINTAINER_BUILD)
    if(KNP_BUILD_AUTONOMOUS)
        find_package(Doxygen OPTIONAL_COMPONENTS dot)
        if(NOT DOXYGEN_FOUND)
            message(WARNING "Doxygen was not found! Documentation build will be disabled by default.")
        endif()
    endif()
endif()

find_program(_STUBGEN stubgen NO_CACHE)

if ("${_STUBGEN}" STREQUAL "_STUBGEN-NOTFOUND")
    message(WARNING "MyPy stubgen was not found! Please install MyPy (see requirements.txt)! Python framework build will be disabled.")
    set(KNP_PYTHON_FRAMEWORK_BUILD_DEFAULT OFF)
else()
    set(KNP_PYTHON_FRAMEWORK_BUILD_DEFAULT ON)
endif()

cmake_dependent_option(KNP_BUILD_DOCUMENTATION "Build doxygen auto documentation" ${KNP_BUILD_AUTONOMOUS} "DOXYGEN_FOUND" OFF)
option(KNP_BUILD_EXAMPLES "Build usage examples" ${KNP_BUILD_AUTONOMOUS})
option(KNP_BUILD_TESTS "Build tests" ${KNP_BUILD_AUTONOMOUS})
option(KNP_ENABLE_AVX "Enable AVX and other CPU-specific extensions in the release build" ${KNP_ENABLE_AVX_DEFAULT})
option(KNP_ENABLE_COVERAGE "Enable coverage checking" OFF)
option(KNP_IPO_ENABLED "Enable interprocedural optimization" ON)
option(KNP_MAINTAINER_BUILD "Build for maintainer, but not for the development purposes" OFF)
cmake_dependent_option(KNP_PYTHON_FRAMEWORK_BUILD "Build KNP Python framework" ON "KNP_PYTHON_FRAMEWORK_BUILD_DEFAULT" OFF)
cmake_dependent_option(KNP_PYTHON_BUILD_WHEEL "Build WHL package for the Python framework" ${KNP_MAINTAINER_BUILD} "KNP_PYTHON_FRAMEWORK_BUILD" OFF)

mark_as_advanced(KNP_MAINTAINER_BUILD)
mark_as_advanced(KNP_ENABLE_COVERAGE)
mark_as_advanced(KNP_ENABLE_AVX)
mark_as_advanced(KNP_IPO_ENABLED)

message(STATUS "KNP_BUILD_DOCUMENTATION = ${KNP_BUILD_DOCUMENTATION}")
message(STATUS "KNP_BUILD_EXAMPLES = ${KNP_BUILD_EXAMPLES}")
message(STATUS "KNP_BUILD_TESTS = ${KNP_BUILD_TESTS}")
message(STATUS "KNP_ENABLE_AVX = ${KNP_ENABLE_AVX}")
message(STATUS "KNP_ENABLE_COVERAGE = ${KNP_ENABLE_COVERAGE}")
message(STATUS "KNP_IPO_ENABLED = ${KNP_IPO_ENABLED}")
message(STATUS "KNP_MAINTAINER_BUILD = ${KNP_MAINTAINER_BUILD}")
message(STATUS "KNP_PYTHON_FRAMEWORK_BUILD = ${KNP_PYTHON_FRAMEWORK_BUILD}")
message(STATUS "KNP_PYTHON_BUILD_WHEEL = ${KNP_PYTHON_BUILD_WHEEL}")

if (KNP_ENABLE_AVX)
    message(STATUS "AVX enabled.")
    message(STATUS "Setting release optimization flags...")
    # -pipe
    set(CMAKE_C_FLAGS_RELEASE "-march=native -mtune=native -mno-vzeroupper -Ofast -funroll-loops -fomit-frame-pointer -finline")

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fopt-info-vec-optimized")
    endif()

    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
    add_compile_options(-include x86intrin.h)
    # -include bits/stdc++.h
endif()

if (NOT KNP_MAINTAINER_BUILD)
    execute_process(COMMAND pre-commit install
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    if(ret AND NOT ret EQUAL 0)
        message(FATAL_ERROR "Can't install pre-commit hooks!")
    endif()
    execute_process(COMMAND pre-commit install -t commit-msg
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

if (KNP_BUILD_TESTS)
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=0)
endif()


#
# Common parameters.
#

set(ENABLE_FAST_MATH ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Need for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Always enabled for the KNP components.
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D_FORTIFY_SOURCE=1")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_FORTIFY_SOURCE=2")

include(knp-functions)
include(print-variables)

#
# Build third-party.
#

set(CPM_SOURCE_CACHE_DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/third-party" CACHE STRING "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/third-party)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/third-party)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/third-party)

# Make cache variables for install destinations.
include(GNUInstallDirs)

add_third_party(NAME "libzmq"
                GITHUB_REPOSITORY "zeromq/libzmq"
                VERSION "4.3.5"
                FIND_PACKAGE_ARGUMENTS "NAMES ZeroMQ libzmq zmq zmq5 libzmq5"
                OPTIONS
                    "WITH_DOCS OFF"
                    "BUILD_TESTS OFF"
                    "ENABLE_CPACK OFF")

#
# LibHDF5 and it's dependencies.
#

add_third_party("https://gitlab.dkrz.de/k202009/libaec.git@1.1.3")
#add_third_party("gh:erdc/szip")

add_third_party("gh:HDFGroup/hdf5#hdf5_1.14.4"
                GIT_SHALLOW ON
                FIND_PACKAGE_ARGUMENTS "NAMES hdf5 libhdf5 hdf5-static"
                OPTIONS
                    "HDF5_NO_PACKAGES ON"
                    "BUILD_SHARED_LIBS ON"
                    "HDF5_VOL_ALLOW_EXTERNAL YES")

add_third_party("gh:BlueBrain/HighFive@2.9.0"
                GIT_SHALLOW ON
                OPTIONS
                    "HIGHFIVE_FIND_HDF5 OFF"
                    "HIGHFIVE_EXAMPLES OFF"
                    "HIGHFIVE_BUILD_DOCS OFF"
                    "HIGHFIVE_USE_BOOST OFF"
                    "HDF5_C_LIBRARIES ${HDF5_LIBRARIES_TO_EXPORT}")

# End of LibHDF5.

add_third_party("gh:p-ranav/csv2@0.1"
                OPTIONS
                    "CSV2_BUILD_TESTS OFF"
                    "CSV2_SAMPLES OFF")

add_third_party("gh:simdjson/simdjson@3.9.2"
                GIT_SHALLOW ON
                OPTIONS
                    "SIMDJSON_BUILD_STATIC_LIB ON")

add_third_party("gh:intel/pcm#202403")

add_third_party("gh:gabime/spdlog@1.13.0"
                OPTIONS
                    "SPDLOG_BUILD_PIC ON")

add_third_party("gh:zeromq/cppzmq@4.10.0"
                OPTIONS
                    "CPPZMQ_BUILD_TESTS OFF")
add_third_party("gh:google/flatbuffers@24.3.25"
                OPTIONS
                    "FLATBUFFERS_BUILD_TESTS OFF"
                    "FLATBUFFERS_BUILD_CPP17 ON")

if (KNP_BUILD_TESTS)
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=0)
    add_third_party("gh:google/googletest@1.14.0")
endif()

file(GLOB THIRD_PARTY_INCLUDES LIST_DIRECTORIES true "third-party/**/include")
include_directories(third-party ${THIRD_PARTY_INCLUDES})


#
# Build KNP.
#

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_TIDY_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/knp/.clang-tidy" CACHE PATH  "Clang-tidy config file")

add_subdirectory("knp")

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

#SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_ICON "doc/html/img/neuromorphic-platform.svg")

set(CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})

set(CPACK_PACKAGE_VENDOR "Kaspersky Laboratory")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Artiom N.")
SET(CPACK_PACKAGE_CONTACT "support@kaspersky.com")

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
