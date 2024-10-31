#[[
© 2024 AO Kaspersky Lab

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
]]

#
# File and directory functions. Artiom N.(cl)2024.
#

include_guard(GLOBAL)


# Function:                 exclude_files_from_dir_in_list
# Description:              Exclude all files from a list under a specific directory.
# Param _InFileList:        Input and return list.
# Param _excludeDirName:    Name of the directory, which shall be ignored.
# Param _verbose:           Print the names of the files handled.
function(exclude_files_from_dir_in_list _InFileList _excludeDirName _verbose)
  foreach (ITR ${_InFileList})
    if ("${_verbose}")
      message(STATUS "ITR=${ITR}")
    endif ("${_verbose}")

    # Check if the item matches the directory name in _excludeDirName.
    if ("${ITR}" MATCHES "(.*)${_excludeDirName}(.*)")
      message(STATUS "Remove Item from List:${ITR}.")
      # Remove the item from the list.
      list (REMOVE_ITEM _InFileList ${ITR})
    endif ("${ITR}" MATCHES "(.*)${_excludeDirName}(.*)")

  endforeach(ITR)

  # Return the SOURCE_FILES variable to the calling parent.
  set(SOURCE_FILES ${_InFileList} PARENT_SCOPE)
endfunction(exclude_files_from_dir_in_list)


# Function:          get_files_and_dir
# Description:       Get files and directories of the source directory in two separate lists.
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
