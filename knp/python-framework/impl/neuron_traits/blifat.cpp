/** @file blifat.cpp
 * @brief BLIFAT neuron Python bindings.
 * @author Artiom N.
 * @date 20.02.24
 */

#if defined(_KNP_IN_NEURON_TRAITS)

#    include "common.h"

using bn_params = knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>;

py::class_<bn_params>("BLIFATNeuronParameters", "Structure for BLIFAT neuron parameters")
    .def_readwrite(
        "n_time_steps_since_last_firing", &bn_params::n_time_steps_since_last_firing_,
        "The parameter defines a number of network steps since the last spike.")
    .def_readwrite(
        "activation_threshold", &bn_params::activation_threshold_,
        "The parameter defines a threshold for membrane potential.")
    .def_readwrite(
        "dynamic_threshold", &bn_params::dynamic_threshold_,
        "The parameter defines a dynamic threshold for membrane potential after reaching which a neuron generates a "
        "spike.")
    .def_readwrite(
        "threshold_decay", &bn_params::threshold_decay_,
        "The parameter defines a time constant during which the `dynamic_threshold_` parameter tends to its base value "
        "if nothing happens.")
    .def_readwrite(
        "threshold_increment", &bn_params::threshold_increment_,
        "The parameter defines a value that increases the `dynamic_threshold_` value if a neuron generates a spike.")
    .def_readwrite(
        "postsynaptic_trace", &bn_params::postsynaptic_trace_,
        "The parameter defines a threshold after reaching which a neuron generates spikes.")
    .def_readwrite(
        "postsynaptic_trace_decay", &bn_params::postsynaptic_trace_decay_,
        "The parameter defines a time constant during which the `postsynaptic_trace_` parameter tends to zero if "
        "nothing happens.")
    .def_readwrite(
        "postsynaptic_trace_increment", &bn_params::postsynaptic_trace_increment_,
        "The parameter defines a value that increases the `postsynaptic_trace_` value if a neuron generates a spike.")
    .def_readwrite(
        "inhibitory_conductance", &bn_params::inhibitory_conductance_,
        "The parameter defines speed with which a potential tends to the `reverse_inhibitory_potential` value.")
    .def_readwrite(
        "inhibitory_conductance_decay", &bn_params::inhibitory_conductance_decay_,
        "The parameter defines a time constant during which the `inhibitory_conductance_` value decreases.")
    .def_readwrite("potential", &bn_params::potential_, "The parameter defines the current membrane potential.")
    .def_readwrite(
        "pre_impact_potential", &bn_params::pre_impact_potential_,
        "This parameter is used if there was a blocking signal. Then all potential changes due to synapses are "
        "ignored.")
    .def_readwrite(
        "potential_decay", &bn_params::potential_decay_,
        "The parameter defines a time constant during which the `potential_` value tends to zero.")
    .def_readwrite(
        "bursting_phase", &bn_params::bursting_phase_,
        "The parameter defines a counter for the `bursting_period_` value.")
    .def_readwrite(
        "bursting_period", &bn_params::bursting_period_,
        "The parameter defines a number of network steps after reaching which a neuron generates a spike.")
    .def_readwrite(
        "reflexive_weight", &bn_params::reflexive_weight_,
        "The parameter defines a value that increases the membrane potential after a neuron generates a spike.")
    .def_readwrite(
        "reversive_inhibitory_potential", &bn_params::reversive_inhibitory_potential_,
        "The parameter takes the default value of `reverse_inhibitory_potential` defined for a BLIFAT neuron.")
    .def_readwrite(
        "absolute_refractory_period", &bn_params::absolute_refractory_period_,
        "The parameter defines a minimum number of network steps before a neuron can generate the next spike.")
    .def_readwrite(
        "potential_reset_value", &bn_params::potential_reset_value_,
        "The parameter defines a potential value after a neuron generates a spike.")
    .def_readwrite(
        "min_potential", &bn_params::min_potential_,
        "The parameter takes the default value of `min_potential` defined for a BLIFAT neuron.")
    .def_readwrite(
        "total_blocking_period", &bn_params::total_blocking_period_,
        "The parameter defines the number of network execution steps, during which the neuron activity is totally "
        "blocked.")
    .def_readwrite(
        "dopamine_value", &bn_params::dopamine_value_,
        "The parameter defines a dopamine value used to sum up all incoming dopamine synapse impacts.");

#endif
