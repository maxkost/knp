/**
 * @file blifat.h
 * @brief BLIFAT neuron types traits.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include <limits>
#include <utility>

#include "type_traits.h"


namespace knp::neuron_traits
{

struct BLIFATNeuron;

template <>
struct neuron_parameters<BLIFATNeuron>
{
    // It means that the neuron never fired (= fired long time ago).
    size_t n_time_steps_since_last_firing_ = std::numeric_limits<size_t>::infinity();
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
    double reversive_inhibitory_potential_ = -0.3;
    unsigned absolute_refractory_period_ = 0;
    double potential_reset_value_ = 0.;
    double min_potential_ = -1.0e9;
};

}  // namespace knp::neuron_traits
