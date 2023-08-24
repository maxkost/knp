/**
 * @file stdp_add_rule.h
 * @brief STDP with additive rule.
 * @author Artiom N.
 * @date 09.08.2023
 */

#pragma once

#include <vector>

#include "type_traits.h"


/**
 * @brief Synapse traits namespace.
 */
namespace knp::synapse_traits
{

/**
 * @brief STDP additive rule parameters.
 * @note Parameters for the W(x) function by Zhang et al. 1998.
 */
template <typename SynapseType>
struct STDPAdditiveRule
{
    using LinkedSynapseType = SynapseType;

    /**
     * @brief Time constant in ms.
     */
    // cppcheck-suppress unusedStructMember
    float tau_plus_ = 10;

    /**
     * @brief Time constant in ms.
     */
    // cppcheck-suppress unusedStructMember
    float tau_minus_ = 10;

    /**
     * @brief Steps when spikes fired.
     */
    // cppcheck-suppress unusedStructMember
    std::vector<size_t> spike_times_;
};

}  // namespace knp::synapse_traits
