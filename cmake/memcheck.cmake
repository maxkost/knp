include_guard(GLOBAL)


find_program(MEMORYCHECK_COMMAND valgrind)
if (MEMORYCHECK_COMMAND)
    message(STATUS "Found Valgrind: ${MEMORYCHECK_COMMAND}")
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")
else()
    message("Valgrind not found")
endif()
