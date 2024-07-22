/**
 * @file synaptic_impact_message.cpp
 * @brief SynapticImpact message Python bindings.
 * @author Artiom N.
 * @date 01.02.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
#include "common.h"


#if defined(_KNP_IN_CORE)

py::class_<core::messaging::SynapticImpact>(
    "SynapticImpact",
    "Structure that contains the synaptic impact value and indexes of presynaptic and posynaptic neurons.");
//        .def(py::self == py::self, "Compare synaptic impact messages.");

py::class_<core::messaging::SynapticImpactMessage>(
    "SynapticImpactMessage", "Structure of the synaptic impact message.");

#endif
