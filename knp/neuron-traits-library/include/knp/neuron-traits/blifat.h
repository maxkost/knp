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

#include "type_traits.h"


namespace knp::neuron_traits
{
struct BLIFATNeuron;

template <>
struct default_values<BLIFATNeuron>
{
    // TODO: see if there are any problems with this implementation, then change it or delete this TODO
    constexpr static std::size_t steps_before_firing = std::numeric_limits<std::size_t>::infinity();
    constexpr static double reverse_inhibitory_potential = -0.3;
    constexpr static double min_potential = -1.0e9;
};


template <>
struct neuron_parameters<BLIFATNeuron>
{
    // It means that the neuron never fired (= fired long time ago).
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
    /// 0 means no bursting.
    unsigned bursting_period = 0;
    double reflexive_weight_ = 0;
    double reversive_inhibitory_potential_ = default_values<BLIFATNeuron>::reverse_inhibitory_potential;
    unsigned absolute_refractory_period_ = 0;
    double potential_reset_value_ = 0.;
    double min_potential_ = default_values<BLIFATNeuron>::min_potential;
};

}  // namespace knp::neuron_traits
