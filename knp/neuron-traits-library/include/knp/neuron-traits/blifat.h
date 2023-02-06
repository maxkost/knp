/**
 * @file blifat.h
 * @brief BLIFAT neuron types traits.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include <string>
#include <tuple>
#include <utility>
#include <numeric>

#include "type_traits.h"


namespace knp::neuron_traits
{
const auto steps_before_firing_starting = std::numeric_limits<size_t>::infinity();
const double default_reverse_inh_potential = -0.3;
const double default_min_potential = -1.0e9;

struct BLIFATNeuron;

template <>
struct neuron_parameters<BLIFATNeuron>
{
    // It means that the neuron never fired (= fired long time ago).
    size_t n_time_steps_since_last_firing_ = steps_before_firing_starting;
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
    double reversive_inhibitory_potential_ = default_reverse_inh_potential;
    unsigned absolute_refractory_period_ = 0;
    double potential_reset_value_ = 0.;
    double min_potential_ = default_min_potential;
};

}  // namespace knp::neuron_traits
