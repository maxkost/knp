/**
 * @file network.cpp
 * @brief Python bindings for Network class.
 * @kaspersky_developer Vartenkov A.
 * @date 05.06.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */


#include "common.h"
#ifdef KNP_IN_BASE_FW


py::class_<knp::framework::Network>("Network")
    .def(py::init<const knp::core::UID&>())
    .def("add_projection", network_add_projection_variant, "Add a projection to the network.")
    .def("add_population", network_add_population_variant, "Add a population to the network.")
    .def("get_population", &network_get_population_variant, "Get a population with the given UID from the network.")
    .def("get_projection", &network_get_projection_variant, "Get a projection with the given UID from the network.")
    .def(
        "remove_population", &knp::framework::Network::remove_population,
        "Remove a population with the given UID from the network.")
    .def(
        "remove_projection", &knp::framework::Network::remove_projection,
        "Remove a projection with the given UID from the network.")
    .add_property("populations_count", &knp::framework::Network::populations_count, "Count populations in the network.")
    .add_property("projections_count", &knp::framework::Network::projections_count, "Count projections in the network.")
    .def("get_uid", &get_entity_uid<knp::framework::Network>, "Get network UID.");

#endif  // KNP_IN_BASE_FW
