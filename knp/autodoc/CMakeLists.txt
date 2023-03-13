cmake_minimum_required(VERSION 3.22) # TODO: increase to 3.25

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(knp-doc)


if(KNP_BUILD_DOCUMENTATION)
    include("doxygen")
    foreach(_i
            core-library base-framework
            neuron-traits-library synapse-traits-library
            devices-library
            backends/cpu-single-threaded-backend
        )
        list(APPEND _strip_list "${CMAKE_SOURCE_DIR}/knp/${_i}/include")
    endforeach()

    add_doxygen(knp-doc "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
        PROJECT_NAME "Kaspersky Neuromorphic Platform"
        PROJECT_VERSION "1.0.0"
        PROJECT_BRIEF "API Reference Guide"
        OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/doc_doxygen"
        INPUT "${CMAKE_SOURCE_DIR}/knp"
        EXAMPLE_PATH "knp/tests"
        PROJECT_LOGO "${CMAKE_CURRENT_SOURCE_DIR}/logo.png"
        SOURCE_BROWSER YES
        BUNDLE_ID "com.kaspersky.KNP"
        PUBLISHER_NAME "Kaspersky Lab"
        EXCLUDE_PATTERNS "${CMAKE_SOURCE_DIR}/knp/tests;${CMAKE_SOURCE_DIR}/knp/.descr;${CMAKE_SOURCE_DIR}/knp/autodoc;.*"
        STRIP_FROM_INC_PATH "${_strip_list}"
    )
    # Automatically build doc.
    add_dependencies(knp-core knp-doc)
endif()
