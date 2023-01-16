# Clang-tidy linter module.
# Artiom N.(cl)2022


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
#  config_path add_params
    if(NOT CLANG_TIDY_COMMAND)
        find_clang_tidy(CLANG_TIDY_COMMAND)
    endif()

    if(CLANG_TIDY_COMMAND MATCHES "cltcache.*")
        set(ENV{CLTCACHE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/cltcache")
    endif()

    get_property(langs GLOBAL PROPERTY ENABLED_LANGUAGES)

    if("CXX" IN_LIST langs)
        list(APPEND TIDY_ARGS "--extra-arg-before=-xc++" "--extra-arg=-std=c++17" ${ARGV2})
    else()
        list(TIDY_ARGS APPEND ${add_params})
    endif()

    if(ARGV1)
        list(APPEND TIDY_ARGS "--config-file=${ARGV1}")
    endif()

    list(APPEND CLANG_TIDY_COMMAND ${CMAKE_CXX_CLANG_TIDY} ${TIDY_ARGS})

    message(STATUS "Setting clang-tidy on \"${target}\": ${CLANG_TIDY_COMMAND}")


    if("${target}" STREQUAL "ALL")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}" CACHE STRING "CMake wrapped clang-tidy")
    else()
        set_target_properties("${target}" PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
    endif()
endfunction()
