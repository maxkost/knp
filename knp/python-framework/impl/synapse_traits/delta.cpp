/**
 * @file delta.cpp
 * @brief Delta synapse Python bindings.
 * @author Artiom N.
 * @date 21.02.24
 */

#if defined(_KNP_IN_SYNAPSE_TRAITS)

#    include "common.h"

py::enum_<knp::synapse_traits::OutputType>("OutputType")
    .value("EXCITATORY", knp::synapse_traits::OutputType::EXCITATORY)  // "Excitatory synapse type."
    .value(
        "INHIBITORY_CURRENT",
        knp::synapse_traits::OutputType::INHIBITORY_CURRENT)  // "Inhibitory by current synapse type"
    .value(
        "INHIBITORY_CONDUCTANCE",
        knp::synapse_traits::OutputType::INHIBITORY_CONDUCTANCE)   // "Inhibitory by conductance synapse type"
    .value("DOPAMINE", knp::synapse_traits::OutputType::DOPAMINE)  // "Dopamine synapse type"
    // "Neuron-blocking synapse type".
    .value("BLOCKING", knp::synapse_traits::OutputType::BLOCKING);

using ds_params = knp::synapse_traits::synapse_parameters<knp::synapse_traits::DeltaSynapse>;

py::class_<ds_params>("DeltaSynapseParameters", "Structure for BLIFAT neuron parameters")
    .def(py::init<>())
    .def(py::init<float, uint32_t, knp::synapse_traits::OutputType>())
    .def_readwrite("weight", &ds_params::weight_, "Synaptic weight.")
    .def_readwrite(
        "delay", &ds_params::delay_,
        "Synaptic delay. Delay of `N` means that a spike sent on step `X` will be received on step `X + N`.")
    .def_readwrite(
        "output_type", &ds_params::output_type_,
        "Synapse type. Various types have different influences on neuron parameters, as defined by neuron function.");

#endif
