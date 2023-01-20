# Clang-tidy linter module.
# Artiom N.(cl)2022

include_guard(GLOBAL)


function(find_clang_tidy result)
    find_program(CLANG_TIDY_CACHE_PATH NAMES "cltcache" "clang-tidy-cache" "cltcache.py"
                 HINTS
                     "${CMAKE_CURRENT_FUNCTION_LIST_DIR}"
                     "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.."
                     "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tools"
    )

    find_program(CLANG_TIDY_PATH
                 NAMES "clang-tidy" "clang-tidy-15" "clang-tidy-14"
                       "clang-tidy-13" "clang-tidy-12"
                 REQUIRED)

    if(CLANG_TIDY_CACHE_PATH)
        set(CLANG_TIDY_PATH "${CLANG_TIDY_CACHE_PATH};${CLANG_TIDY_PATH}")
    endif()

   set("${result}" "${CLANG_TIDY_PATH}" PARENT_SCOPE)
endfunction()


function(add_clang_tidy target)
#  CONFIG_FILE_PATH <config> [add_params]
    cmake_parse_arguments(
        PARSED_ARGS
        ""
        "CONFIG_FILE_PATH"
        "EXTRA_ARGS"
        ${ARGN}
    )

    if(NOT CLANG_TIDY_COMMAND)
        find_clang_tidy(CLANG_TIDY_COMMAND)
    endif()

    if(CLANG_TIDY_COMMAND MATCHES "cltcache.*")
        set(ENV{CLTCACHE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/cltcache")
    endif()

    get_property(langs GLOBAL PROPERTY ENABLED_LANGUAGES)

    if("CXX" IN_LIST langs)
        list(APPEND TIDY_ARGS "--extra-arg-before=-xc++" "--extra-arg=-std=c++17" ${PARSED_ARGS_EXTRA_ARGS})
    else()
        list(TIDY_ARGS APPEND ${add_params})
    endif()

    if(PARSED_ARGS_CONFIG_FILE_PATH)
        list(APPEND TIDY_ARGS "--config-file=${PARSED_ARGS_CONFIG_FILE_PATH}")
    endif()

    list(APPEND CLANG_TIDY_COMMAND ${CMAKE_CXX_CLANG_TIDY} ${TIDY_ARGS})

    message(STATUS "Setting clang-tidy on \"${target}\": ${CLANG_TIDY_COMMAND}")

    # Temporary skip.
    return()

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # cstddef:50:10: error: 'stddef.h' file not found [clang-diagnostic-error].
        message(WARNING "Clang-tidy was disabled because of the bug!")
        return()
    endif()

    if("${target}" STREQUAL "ALL")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}" CACHE STRING "CMake wrapped clang-tidy")
    else()
        set_target_properties("${target}" PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
    endif()
endfunction()
