/**
 * @file network_io.cpp
 * @brief Python bindings for saving and loading network.
 * @author Vartenkov A.
 * @date 05.06.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
#include "common.h"

#ifdef KNP_IN_BASE_FW

py::def("save_network", &knp::framework::sonata::save_network, "Save network to disk.");

py::def("load_network", &knp::framework::sonata::load_network, "Load network from disk.");

#endif  // KNP_IN_BASE_FW
