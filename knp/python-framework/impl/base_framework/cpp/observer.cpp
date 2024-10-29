/**
 * @file observer.cpp
 * @brief Python bindings for MessageObserver.
 * @kaspersky_developer Vartenkov A.
 * @date 05.06.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include "common.h"


#ifdef KNP_IN_BASE_FW

using SpikeMsg = knp::core::messaging::SpikeMessage;
using ImpactMsg = knp::core::messaging::SynapticImpactMessage;
using SpikeObserver = knp::framework::monitoring::MessageObserver<SpikeMsg>;
using ImpactObserver = knp::framework::monitoring::MessageObserver<ImpactMsg>;

py::def("make_spike_observer", make_observer<SpikeMsg>, "Create an observer to process spike messages");

py::def("make_impact_observer", make_observer<ImpactMsg>, "Create an observer to process synaptic impacts");


py::class_<SpikeObserver, boost::noncopyable>(
    "SpikeMessageObserver",
    "The SpikeMessageObserver class is a definition of an observer that receives spike messages and processes them.",
    py::no_init)
    .def("get_uid", &get_entity_uid<SpikeObserver>, "Get observer UID.")
    .def("subscribe", &SpikeObserver::subscribe, "Subscribe to spike messages from a list of entities.");

py::class_<ImpactObserver, boost::noncopyable>(
    "ImpactMessageObserver",
    "The ImpactMessageObserver class is an observer that receives synaptic impact messages and processes them.",
    py::no_init)
    .def("get_uid", &get_entity_uid<ImpactObserver>, "Get observer UID.")
    .def("subscribe", &ImpactObserver::subscribe, "Subscribe to synaptic impact messages from a list of entities.");
#endif  // KNP_IN_BASE_FW
