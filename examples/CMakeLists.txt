cmake_minimum_required(VERSION 3.25)

include(GNUInstallDirs)
include(knp-functions)

set(KNP_ENABLE_COVERAGE OFF)
set(CMAKE_CXX_STANDARD 17)

#add_subdirectory(digits-recognition)
add_subdirectory(simple-network)

install(DIRECTORY "."
        COMPONENT "examples"
        DESTINATION "src/knp/examples")

knp_packaging_set_parameters(examples examples
        DESCRIPTION "KNP examples"
        DEPENDS framework-dev)
