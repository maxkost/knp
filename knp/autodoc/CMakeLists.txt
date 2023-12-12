cmake_minimum_required(VERSION 3.22) # TODO: increase to 3.25

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(knp-doc)

file(REAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../knp" KNP_DIR)

message(DEBUG "KNP directory: ${KNP_DIR}")


if(KNP_BUILD_DOCUMENTATION)
    include("doxygen")
    foreach(_i
            core-library base-framework
            neuron-traits-library synapse-traits-library
            devices-library
            backends/cpu-single-threaded-backend
        )
        list(APPEND _strip_list "${KNP_DIR}/${_i}/include")
    endforeach()

    add_doxygen(knp-doc "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
        PROJECT_NAME "Kaspersky Neuromorphic Platform"
        PROJECT_VERSION "1.0.0"
        PROJECT_BRIEF "API Reference Guide"
        OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/doc_doxygen"
        INPUT "${KNP_DIR}"
        EXAMPLE_PATH "${KNP_DIR}/tests"
        SOURCE_BROWSER YES
        BUNDLE_ID "com.kaspersky.KNP"
        PUBLISHER_NAME "Kaspersky Lab"
        EXCLUDE_PATTERNS "*.descr* */impl/*"
        EXCLUDE_SYMBOLS "*impl*, *testing*"
        STRIP_FROM_INC_PATH "${_strip_list}"
    )
    # Automatically build doc.
    add_dependencies(knp-core knp-doc)
endif()
