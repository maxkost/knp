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

# OCLint tool wrapper.
# Artiom N.(cl)2023

include_guard(GLOBAL)


function(download_and_extract url out_path)
    get_filename_component(archive_name "${url}" NAME)
    set(archive "${out_path}/${archive_name}")
    file(DOWNLOAD "${url}" "${archive}" SHOW_PROGRESS TLS_VERIFY ON)
    message(STATUS "Extracting ${archive}...")
    file(ARCHIVE_EXTRACT INPUT "${archive}" VERBOSE DESTINATION "${out_path}/")
    file(REMOVE "${archive}")
endfunction()


function(download_oclint url version out_path)
    # message(FATAL_ERROR "SSS ${CMAKE_SYSTEM_PROCESSOR}")
    if(NOT EXISTS "${out_path}/oclint-${version}")
        download_and_extract("${url}" "${out_path}")
    endif()
endfunction()


function(run_oclint)

endfunction()
