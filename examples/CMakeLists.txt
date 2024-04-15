cmake_minimum_required(VERSION 3.25)

include(GNUInstallDirs)
include(knp-functions)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(arni-save-load)
add_subdirectory(save-load-networks)
add_subdirectory(simple-network)

install(DIRECTORY "."
        COMPONENT "examples"
        DESTINATION "src/knp/examples")

knp_packaging_set_parameters(examples examples
        DESCRIPTION "KNP examples"
        DEPENDS framework-dev)
