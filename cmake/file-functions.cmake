#
# File and directory functions. Artiom N.(cl)2024.
#

include_guard(GLOBAL)


# Function:                 exclude_files_from_dir_in_list
# Description:              Exclude all files from a list under a specific directory.
# Param _InFileList:        Input and returned List
# Param _excludeDirName:    Name of the directory, which shall be ignored.
# Param _verbose:           Print the names of the files handled
function(exclude_files_from_dir_in_list _InFileList _excludeDirName _verbose)
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
endfunction(exclude_files_from_dir_in_list)


# Function:          get_files_and_dir
# Description:       Get files and directories of the source directory in the two separate lists.
# Param source_dir:  Directory to list.
# output dir_list:   Directories in the source directory.
# output file_list:  Files in the source directory.
function(get_files_and_dir source_dir dir_list file_list)
    file(GLOB nodes LIST_DIRECTORIES YES ${source_dir}/*)

    set(_dir_list "")
    set(_file_list "")

    foreach(_node ${nodes})
        if(IS_DIRECTORY "${_node}")
            list(APPEND _dir_list "${_node}")
        else()
            list(APPEND _file_list "${_node}")
        endif()
    endforeach()

    set(${dir_list} ${_dir_list} PARENT_SCOPE)
    set(${file_list} ${_file_list} PARENT_SCOPE)
endfunction(get_files_and_dir)
