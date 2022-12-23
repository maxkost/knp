function(snn_capitalize_string src result)
    # Get first letter and capitalize.
    string(SUBSTRING ${src} 0 1 first-letter)
    string(TOUPPER ${first-letter} first-letter)
    # Get the rest of the macro name.
    string(LENGTH ${src} length)
    string(SUBSTRING ${src} 1 ${length} rest)
    # Compose macro-name with first letter uppercase.
    set(${result} "${first-letter}${rest}" PARENT_SCOPE)
endfunction()


function (snn_use_backend BACKEND_DIRECTORY)
    set("${BACKEND_DIRECTORY}" PARENT_SCOPE)
    set(BACKEND_SUBDIR "${CMAKE_CURRENT_SOURCE_DIR}/src/impl/backends/${BACKEND_DIRECTORY}")
    add_subdirectory("${BACKEND_SUBDIR}")
endfunction()


function (snn_set_backend_common_parameters BACKEND_NAME PROJECT_ROOT_DIR)
    string(TOLOWER "${BACKEND_NAME}" BACKEND_DIRECTORY)
    file(REAL_PATH "${PROJECT_ROOT_DIR}/src" PROJECT_SOURCE_DIR)
    file(REAL_PATH "${PROJECT_SOURCE_DIR}/impl/backends/${BACKEND_DIRECTORY}" BACKEND_SUBDIR)
    snn_capitalize_string(${BACKEND_DIRECTORY} BACKEND_CLASS_PREFIX)
    target_compile_definitions("snn_backend_${BACKEND_DIRECTORY}" PRIVATE "-D_BUILD_LIBRARY" PUBLIC "-DSNN_LIBRARY_BACKEND_NAME=\"${BACKEND_NAME}\"" "-DSNN_LIBRARY_BACKEND_CLASS=calculation_backend::${BACKEND_CLASS_PREFIX}CalculationBackend")
    target_include_directories("snn_backend_${BACKEND_DIRECTORY}" PRIVATE "${BACKEND_SUBDIR}")
    target_include_directories("snn_backend_${BACKEND_DIRECTORY}" PUBLIC "${PROJECT_SOURCE_DIR}/include")
    target_include_directories("snn_backend_${BACKEND_DIRECTORY}" PUBLIC "${PROJECT_SOURCE_DIR}/include/snn-library/backends/${BACKEND_DIRECTORY}")
    target_compile_features("${PROJECT_NAME}" PUBLIC cxx_std_17)
    string(TOUPPER "${BACKEND_NAME}" BACKEND_NAME_UPPER)
    set("BUILD_${BACKEND_NAME_UPPER}_BACKEND" 1 CACHE INTERNAL "")
endfunction()


macro (snn_set_cmake_common_parameters)
    # find_package(Intl REQUIRED)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

    find_package(Boost 1.66.0 REQUIRED)

    if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
    endif()

    #
    # SDL requirements.
    #

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS} -Wall -fPIE -fstack-protector-all -Wformat -Wformat-security")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -Wall -fPIE -fstack-protector-all -Wformat -Wformat-security")

    #
    # Coverage. TODO: Doesn't work with OpenAcc, fix it.
    #

    # set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")

    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D_FORTIFY_SOURCE=2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_FORTIFY_SOURCE=2")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

    # Sanitizer (dynamic analysis).
    # Sanitizers don't work under TFS. Temporarily disabled.

    # set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize-recover=address")
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize-recover=address")
    if(UNIX AND NOT APPLE)
#        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,noexecstack -Wl,-z,relro,-z,now")
    endif()
endmacro()


# Function:                 EXCLUDE_FILES_FROM_DIR_IN_LIST
# Description:              Exclude all files from a list under a specific directory.
# Param _InFileList:        Input and returned List
# Param _excludeDirName:    Name of the directory, which shall be ignored.
# Param _verbose:           Print the names of the files handled

function(EXCLUDE_FILES_FROM_DIR_IN_LIST _InFileList _excludeDirName _verbose)
  foreach (ITR ${_InFileList})
    if ("${_verbose}")
      message(STATUS "ITR=${ITR}")
    endif ("${_verbose}")

    if ("${ITR}" MATCHES "(.*)${_excludeDirName}(.*)")                   # Check if the item matches the directory name in _excludeDirName
      message(STATUS "Remove Item from List:${ITR}")
      list (REMOVE_ITEM _InFileList ${ITR})                              # Remove the item from the list
    endif ("${ITR}" MATCHES "(.*)${_excludeDirName}(.*)")

  endforeach(ITR)
  set(SOURCE_FILES ${_InFileList} PARENT_SCOPE)                          # Return the SOURCE_FILES variable to the calling parent
endfunction(EXCLUDE_FILES_FROM_DIR_IN_LIST)


function(CUDA_CONVERT_FLAGS EXISTING_TARGET)
    get_property(old_flags TARGET ${EXISTING_TARGET} PROPERTY INTERFACE_COMPILE_OPTIONS)
    if(NOT "${old_flags}" STREQUAL "")
        string(REPLACE ";" "," CUDA_flags "${old_flags}")
        set_property(TARGET ${EXISTING_TARGET} PROPERTY INTERFACE_COMPILE_OPTIONS
            "$<$<BUILD_INTERFACE:$<COMPILE_LANGUAGE:CXX>>:${old_flags}>$<$<BUILD_INTERFACE:$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=${CUDA_flags}>"
            )
    endif()
endfunction()
