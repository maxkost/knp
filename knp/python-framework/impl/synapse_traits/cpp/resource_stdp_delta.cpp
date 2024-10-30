/**
 * @file resource_stdp_delta.cpp
 * @brief Python bindings for resource stdp delta synapse.
 * @author Vartenkov A.
 * @date 25.10.24
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#if defined(_KNP_IN_SYNAPSE_TRAITS)
using rds_params = knp::synapse_traits::synapse_parameters<knp::synapse_traits::SynapticResourceSTDPDeltaSynapse>;
using rds_rule = knp::synapse_traits::STDPSynapticResourceRule<knp::synapse_traits::DeltaSynapse>;


py::class_<rds_rule>(
    "SynapticResourceSTDPDeltaSynapseRule", "Parameters for synaptic resource delta synapse plasticity.")
    .def(py::init<>())
    .add_property(
        "synaptic_resource", &rds_rule::synaptic_resource_, "Synaptic resource that determines synapse weight")
    .add_property("min_weight", &rds_rule::w_min_, "Minimal weight value.")
    .add_property("max_weight", &rds_rule::w_max_, "Maximal weight value.")
    .add_property("d_u", &rds_rule::d_u_, "Resource decreasing constant.")
    .add_property(
        "dopamine_plasticity_period", &rds_rule::dopamine_plasticity_period_,
        "Dopamine plasticity period. If a neuron is rewarded during this period, then the synapse weight changes.")
    .add_property("last_spike_step", &rds_rule::last_spike_step_, "Step number of when the synapse got a spike last.")
    .add_property(
        "had_hebbian_update", &rds_rule::had_hebbian_update_,
        "True if the synapse was updated during the current spike sequence.");


py::class_<rds_params, py::bases<ds_params>>(
    "SynapticResourceSTDPDeltaSynapseParameters", "Structure for Resource STDP Delta synapse parameters.")
    .def(py::init<>())
    .add_property("weight", &rds_params::weight_, "Synaptic weight.")
    .add_property(
        "delay", &rds_params::delay_,
        "Synaptic delay. Delay of `N` means that a spike sent on step `X` will be received on step `X + N`.")
    .add_property(
        "output_type", &rds_params::output_type_,
        "Synapse type. Various types have different influences on neuron parameters as defined by neuron function.")
    .add_property("rule", &rds_params::rule_, "Structure containing synaptic plasticity parameters.");


#endif
