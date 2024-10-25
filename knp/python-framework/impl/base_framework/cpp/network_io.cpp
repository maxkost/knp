//
// Created by an_vartenkov on 07.06.24.
//
#include "common.h"

#ifdef KNP_IN_BASE_FW

py::def("save_network", &knp::framework::sonata::save_network, "Save network to disk.");

py::def("load_network", &knp::framework::sonata::load_network, "Load network from disk.");

#endif  // KNP_IN_BASE_FW
