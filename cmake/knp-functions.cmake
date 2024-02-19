#
# KNP build functions. Artiom N.(cl)2023.
#


include_guard(GLOBAL)

function(knp_capitalize_string src result)
    # Get first letter and capitalize.
    string(SUBSTRING ${src} 0 1 first-letter)
    string(TOUPPER ${first-letter} first-letter)
    # Get the rest of the macro name.
    string(LENGTH ${src} length)
    string(SUBSTRING ${src} 1 ${length} rest)
    # Compose macro-name with first letter uppercase.
    set(${result} "${first-letter}${rest}" PARENT_SCOPE)
endfunction()


function(knp_set_target_parameters target)
    target_compile_options("${target}" PRIVATE $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wdocumentation>)
    target_compile_options("${target}" PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Wdocumentation>)
    # SDL requirements.
    target_compile_options("${target}" PUBLIC $<$<COMPILE_LANG_AND_ID:C,GNU,Clang>:-Wall -fstack-protector-all -Wformat -Wformat-security>)
    target_compile_options("${target}" PUBLIC $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wall -fstack-protector-all -Wformat -Wformat-security>)

    target_compile_definitions("${target}" PRIVATE "KNP_LIBRARY_NAME=${target}")

    target_include_directories("${target}" PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
endfunction()


function (knp_add_library lib_name lib_type)
    set(options "")
    set(one_value_args "ALIAS")
    set(multi_value_args "")
    cmake_parse_arguments(LIBRARY_PARAMS "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    set(${lib_name}_source ${ARGN})

    string(TOUPPER "${lib_type}" lib_type)

    if(NOT lib_type OR lib_type STREQUAL "BOTH")
        # Build common object library.
        add_library("${lib_name}_obj" OBJECT ${${lib_name}_source})
        set_target_properties("${lib_name}_obj" PROPERTIES POSITION_INDEPENDENT_CODE ON)

        add_library("${lib_name}" SHARED $<TARGET_OBJECTS:${lib_name}_obj>)
        knp_set_target_parameters("${lib_name}")
        target_compile_definitions("${lib_name}" PRIVATE "KNP_LIBRARY_NAME_PREFIX=${CMAKE_SHARED_LIBRARY_PREFIX}")
        target_compile_definitions("${lib_name}" PRIVATE "KNP_FULL_LIBRARY_NAME=${CMAKE_SHARED_LIBRARY_PREFIX}${lib_name}")

        add_library("${lib_name}_static" STATIC $<TARGET_OBJECTS:${lib_name}_obj>)
        knp_set_target_parameters("${lib_name}_static")
        target_compile_definitions("${lib_name}_static" PRIVATE "KNP_LIBRARY_NAME_PREFIX=${CMAKE_STATIC_LIBRARY_PREFIX}")
        target_compile_definitions("${lib_name}_static" PRIVATE "KNP_FULL_LIBRARY_NAME=${CMAKE_STATIC_LIBRARY_PREFIX}${lib_name}")

        set_target_properties("${lib_name}_static" PROPERTIES OUTPUT_NAME "${lib_name}")
    elseif(lib_type STREQUAL SHARED OR lib_type STREQUAL MODULE)
        add_library("${lib_name}" ${lib_type} ${${lib_name}_source})
        target_compile_definitions("${lib_name}" PRIVATE BUILD_SHARED_LIBS)
        knp_set_target_parameters("${lib_name}")
        target_compile_definitions("${lib_name}" PRIVATE "KNP_LIBRARY_NAME_PREFIX=${CMAKE_SHARED_LIBRARY_PREFIX}")
        target_compile_definitions("${lib_name}" PRIVATE "KNP_FULL_LIBRARY_NAME=${CMAKE_SHARED_LIBRARY_PREFIX}${lib_name}")
    elseif(lib_type STREQUAL STATIC)
        add_library("${lib_name}" STATIC ${${lib_name}_source})
        knp_set_target_parameters("${lib_name}")
        target_compile_definitions("${lib_name}" PRIVATE "KNP_LIBRARY_NAME_PREFIX=${CMAKE_STATIC_LIBRARY_PREFIX}")
        target_compile_definitions("${lib_name}" PRIVATE "KNP_FULL_LIBRARY_NAME=${CMAKE_STATIC_LIBRARY_PREFIX}${lib_name}")
    else()
        message(FATAL_ERROR "Incorrect library build type: \"${lib_type}\". Use SHARED/MODULE, STATIC or BOTH.")
    endif()
endfunction()


function (knp_set_backend_common_parameters BACKEND_NAME PROJECT_ROOT_DIR)
    string(TOLOWER "${BACKEND_NAME}" BACKEND_DIRECTORY)
    file(REAL_PATH "${PROJECT_ROOT_DIR}/src" PROJECT_SOURCE_DIR)
    file(REAL_PATH "${PROJECT_SOURCE_DIR}/impl/backends/${BACKEND_DIRECTORY}" BACKEND_SUBDIR)
    knp_capitalize_string(${BACKEND_DIRECTORY} BACKEND_CLASS_PREFIX)
    target_compile_definitions("knp_backend_${BACKEND_DIRECTORY}" PRIVATE "-D_BUILD_LIBRARY" PUBLIC "-DKNP_LIBRARY_BACKEND_NAME=\"${BACKEND_NAME}\"" "-DKNP_LIBRARY_BACKEND_CLASS=calculation_backend::${BACKEND_CLASS_PREFIX}CalculationBackend")
    target_include_directories("knp_backend_${BACKEND_DIRECTORY}" PRIVATE "${BACKEND_SUBDIR}")
    target_include_directories("knp_backend_${BACKEND_DIRECTORY}" PUBLIC "${PROJECT_SOURCE_DIR}/include")
    target_include_directories("knp_backend_${BACKEND_DIRECTORY}" PUBLIC "${PROJECT_SOURCE_DIR}/include/snn-library/backends/${BACKEND_DIRECTORY}")
    target_compile_features("${PROJECT_NAME}" PUBLIC cxx_std_17)
    string(TOUPPER "${BACKEND_NAME}" BACKEND_NAME_UPPER)
    set("BUILD_${BACKEND_NAME_UPPER}_BACKEND" 1 CACHE INTERNAL "")
endfunction()


macro (knp_set_cmake_common_parameters)
    # find_package(Intl REQUIRED)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

    find_package(Boost 1.66.0 REQUIRED)

    if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
    endif()


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

    # Check if the item matches the directory name in _excludeDirName.
    if ("${ITR}" MATCHES "(.*)${_excludeDirName}(.*)")
      message(STATUS "Remove Item from List:${ITR}")
      # Remove the item from the list.
      list (REMOVE_ITEM _InFileList ${ITR})
    endif ("${ITR}" MATCHES "(.*)${_excludeDirName}(.*)")

  endforeach(ITR)

  # Return the SOURCE_FILES variable to the calling parent.
  set(SOURCE_FILES ${_InFileList} PARENT_SCOPE)
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


# Function:                 knp_add_python_library
# Description:              Add Python module, using Boost::Python.
# Param name:               Module name.
# Param LINK_LIBRARIES:     Additional libraries to link.
# Requires:
#     find_package(Python COMPONENTS Interpreter Development REQUIRED)
#     find_package(Boost 1.66.0 COMPONENTS python REQUIRED)

function(knp_add_python_module name)
    cmake_parse_arguments(
            "PARSED_ARGS"
            ""
            ""
            "LINK_LIBRARIES"
            ${ARGN}
    )

    set(LIB_NAME "${PROJECT_NAME}_${name}")

    file(GLOB_RECURSE ${name}_MODULES_SOURCE CONFIGURE_DEPENDS
            "impl/${name}/*.h"
            "impl/${name}/*.cpp")

    knp_add_library(
            "${LIB_NAME}"
            SHARED
            ${${name}_MODULES_SOURCE}
    )

    target_include_directories("${LIB_NAME}" PRIVATE ${Python_INCLUDE_DIRS})
    target_link_libraries("${LIB_NAME}" PRIVATE Boost::headers Boost::python ${PARSED_ARGS_LINK_LIBRARIES})
endfunction()
