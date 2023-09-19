/**
 * @file stdp_synaptic_resource_rule.h
 * @brief STDP rule with synaptic resource.
 * @author Artiom N.
 * @date 05.09.2023
 */

#pragma once

#include <cinttypes>
#include <vector>

#include "type_traits.h"


/**
 * @brief Synapse traits namespace.
 */
namespace knp::synapse_traits
{

/**
 * @brief STDP rule with synaptic resource parameters.
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
    float synaptic_resource_;
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
};

}  // namespace knp::synapse_traits
