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
};

}  // namespace knp::synapse_traits
