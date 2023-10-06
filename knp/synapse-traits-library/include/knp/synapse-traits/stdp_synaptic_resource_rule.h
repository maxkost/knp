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
    // cppcheck-suppress unusedStructMember
    float synaptic_resource_ = 1;
    /**
     * @brief Minimal weight value.
     */
    // cppcheck-suppress unusedStructMember
    float w_min_;
    /**
     * @brief Maximal weight value.
     */
    // cppcheck-suppress unusedStructMember
    float w_max_;
    /**
     * @brief Resource decreasing constant.
     * @note d_u_ must be equal or greate 0.
     */
    // cppcheck-suppress unusedStructMember
    float d_u_;
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
