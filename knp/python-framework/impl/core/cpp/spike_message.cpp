/**
 * @file spike_message.cpp
 * @brief Python bindings for SpikeMessage.
 * @author Artiom N.
 * @date 01.02.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#if defined(KNP_IN_CORE)

#    include "spike_message.h"

#    include "common.h"


py::class_<core::messaging::SpikeData>("SpikeData", "List of spike indexes.")
    .def(py::vector_indexing_suite<core::messaging::SpikeData>());


py::class_<core::messaging::SpikeMessage>("SpikeMessage", "Structure of the spike message.")
    .def("__init__", py::make_constructor(&spike_message_constructor), "Constract a spike message.")
    .def_readwrite("header", &core::messaging::SpikeMessage::header_, "Message header.")
    .def_readwrite(
        "neuron_indexes", &core::messaging::SpikeMessage::neuron_indexes_, "Indexes of the recently spiked neurons.")
    .def(py::self_ns::str(py::self));

#endif
