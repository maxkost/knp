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
    constexpr static std::size_t steps_before_firing = std::numeric_limits<std::size_t>::infinity();
    constexpr static double reverse_inhibitory_potential = -0.3;
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
    double dynamic_threshold_ = 0.;
    double threshold_decay_ = 0.;
    double threshold_increment_ = 0.;
    double postsynaptic_trace_ = 0.;
    double postsynaptic_trace_decay_ = 0.;

    double postsynaptic_trace_increment_ = 0.;
    double inhibitory_conductance_ = 0.;
    double inhibitory_conductance_decay_ = 0.;
    double potential_ = 0;
    double potential_decay_ = 0;

    unsigned bursting_phase_ = 0;

    /**
     * @brief Value of 0 means that no bursting occurs.
     */
    unsigned bursting_period_ = 0;
    double reflexive_weight_ = 0;

    /**
     * @brief The parameter takes the default value of `reverse_inhibitory_potential` defined for a BLIFAT neuron.
     */
    double reversive_inhibitory_potential_ = default_values<BLIFATNeuron>::reverse_inhibitory_potential;
    unsigned absolute_refractory_period_ = 0;
    double potential_reset_value_ = 0.;

    /**
     * @brief The parameter takes the default value of `min_potential` defined for a BLIFAT neuron.
     */
    double min_potential_ = default_values<BLIFATNeuron>::min_potential;
    /**
     * @brief The parameter defines the number of network execution steps, during which the neuron activity is totally blocked.
     */
    unsigned total_blocking_period_ = 0;
};

}  // namespace knp::neuron_traits
