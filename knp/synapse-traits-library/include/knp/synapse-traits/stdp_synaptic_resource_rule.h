/**
 * @file stdp_synaptic_resource_rule.h
 * @brief STDP rule with synaptic resource.
 * @author Artiom N.
 * @date 05.09.2023
 */

#pragma once

#include <cinttypes>
#include <vector>

#include "stdp_common.h"


/**
 * @brief Synapse traits namespace.
 */
namespace knp::synapse_traits
{

/**
 * @brief STDP rule with synaptic resource parameters.
 * @tparam SynapseType synapse type linked with rule.
 * @note Author of the synaptic resource - Mikhail Kiselev.
 */
template <typename SynapseType>
struct STDPSynapticResourceRule
{
    /**
     * @brief Type of the synapse linked with rule.
     */
    using LinkedSynapseType = SynapseType;

    /**
     * @brief Synaptic resource.
     */
    float synaptic_resource_ = 0;
    /**
     * @brief Minimal weight value.
     */
    float w_min_ = 0;
    /**
     * @brief Maximal weight value.
     */
    float w_max_ = 1;
    /**
     * @brief Resource decreasing constant.
     * @note d_u_ must be equal or greate 0.
     */
    float d_u_ = 0;

    /**
     * @brief Dopamine plasticity period: if a neuron is rewarded during this period, synapse weight is changed.
     */
    uint32_t dopamine_plasticity_period_ = 0;

    /**
     * @brief Last time this synapse got a spike.
     */
    uint64_t last_spike_step_ = 0;
};


/**
 * @brief Shared parameters for the resource STDP.
 * @tparam SynapseType synapse type linked with resource-based STDP rule.
 */
template <typename SynapseType>
struct shared_synapse_parameters<STDP<STDPSynapticResourceRule, SynapseType>>
{
};

}  // namespace knp::synapse_traits
