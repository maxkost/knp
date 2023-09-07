/**
 * @file blifat.h
 * @brief BLIFAT neuron types traits.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include <limits>
#include <numeric>
#include <utility>

#include <boost/mp11.hpp>

#include "type_traits.h"

/**
 * @brief Neuron traits namespace.
 */
namespace knp::neuron_traits
{

/**
 * @brief BLIFAT neuron. Use as a template parameter only.
 */
struct BLIFATNeuron;

/**
 * @brief Structure for BLIFAT neuron default values.
 */
template <>
struct default_values<BLIFATNeuron>
{
    // TODO: see if there are any problems with this implementation, then change it or delete this TODO
    /**
     * @brief The parameter defines a number of network steps since the last spike.
     */
    constexpr static std::size_t steps_before_firing = std::numeric_limits<std::size_t>::infinity();
    /**
     * @brief The parameter defines a value to which membrane potential tends (for conductance-based inhibitory
     * synapses)
     */
    constexpr static double reverse_inhibitory_potential = -0.3;
    /**
     * @brief The parameter defines a value to which membrane potential tends (for current-based inhibitory synapses).
     */
    constexpr static double min_potential = -1.0e9;
};

/**
 * @brief Structure for BLIFAT neuron parameters.
 */
template <>
struct neuron_parameters<BLIFATNeuron>
{
    /**
     * @brief The parameter takes the default value of `steps_before_firing` defined for a BLIFAT neuron.
     */
    std::size_t n_time_steps_since_last_firing_ = default_values<BLIFATNeuron>::steps_before_firing;

    /**
     * @brief The parameter defines a threshold for membrane potential.
     */
    double activation_threshold_ = 1.0;
    /**
     * @brief The parameter defines a dynamic threshold for membrane potential after reaching which a neuron generates a
     * spike.
     */
    double dynamic_threshold_ = 0.;
    /**
     * @brief The parameter defines a time constant during which the `dynamic_threshold_` parameter tends to its base
     * value if nothing happens.
     */
    double threshold_decay_ = 0.;
    /**
     * @brief The parameter defines a value that increases the `dynamic_threshold_` value if a neuron generates a spike.
     */
    double threshold_increment_ = 0.;
    /**
     * @brief The parameter defines a threshold after reaching which a neuron generates spikes.
     */
    double postsynaptic_trace_ = 0.;
    /**
     * @brief The parameter defines a time constant during which the `postsynaptic_trace_` parameter tends to zero if
     * nothing happens.
     * @details If `postsynaptic_trace_decay_` equals 0, then `postsynaptic_trace_` also equals 0.
     */
    double postsynaptic_trace_decay_ = 0.;

    /**
     * @brief The parameter defines a value that increases the `postsynaptic_trace_` value if a neuron generates a
     * spike.
     */
    double postsynaptic_trace_increment_ = 0.;
    /**
     * @brief The parameter defines speed with which a potential tends to the `reverse_inhibitory_potential` value.
     */
    double inhibitory_conductance_ = 0.;

    /**
     * @brief The parameter defines a time constant during which the `inhibitory_conductance_` value decreases.
     */
    double inhibitory_conductance_decay_ = 0.;
    /**
     * @brief The parameter defines the current membrane potential.
     */
    double potential_ = 0;

    /**
     * @brief This parameter is used if there was a blocking signal. Then all potential changes due to synapses are
     * ignored.
     */
    double pre_impact_potential_ = 0;
    /**
     * @brief The parameter defines a time constant during which the `potential_` value tends to zero.
     */
    double potential_decay_ = 0;

    /**
     * @brief The parameter defines a counter for the `bursting_period_` value.
     */
    unsigned bursting_phase_ = 0;

    /**
     * @brief The parameter defines a number of network steps after reaching which a neuron generates a spike.
     * @details Value of 0 means that no bursting occurs.
     */
    unsigned bursting_period_ = 0;
    /**
     * @brief The parameter defines a value that increases the membrane potential after a neuron generates a spike.
     */
    double reflexive_weight_ = 0;

    /**
     * @brief The parameter takes the default value of `reverse_inhibitory_potential` defined for a BLIFAT neuron.
     */
    double reversive_inhibitory_potential_ = default_values<BLIFATNeuron>::reverse_inhibitory_potential;
    /**
     * @brief The parameter defines a minimum number of network steps before a neuron can generate the next spike.
     */
    unsigned absolute_refractory_period_ = 0;
    /**
     * @brief The parameter defines a potential value after a neuron genarates a spike.
     */
    double potential_reset_value_ = 0.;

    /**
     * @brief The parameter takes the default value of `min_potential` defined for a BLIFAT neuron.
     */
    double min_potential_ = default_values<BLIFATNeuron>::min_potential;
    /**
     * @brief The parameter defines the number of network execution steps, during which the neuron activity is totally
     * blocked.
     */
    unsigned total_blocking_period_ = 0;
};

}  // namespace knp::neuron_traits
