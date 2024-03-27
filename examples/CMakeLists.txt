cmake_minimum_required(VERSION 3.25)


set(KNP_ENABLE_COVERAGE OFF)

#add_subdirectory(digits-recognition)
add_subdirectory(simple-network)

set(CPACK_DEBIAN_EXAMPLES_DESCRIPTION "KNP examples" PARENT_SCOPE)
set(CPACK_DEBIAN_EXAMPLES_PACKAGE_DEPENDS "knp (== ${KNP_VERSION})" PARENT_SCOPE)
