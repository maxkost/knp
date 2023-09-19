/**
 * @file stdp_add_rule.h
 * @brief STDP with additive rule.
 * @author Artiom N.
 * @date 09.08.2023
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
 * @brief STDP additive rule parameters.
 * @note Parameters for the W(x) function by Zhang et al. 1998.
 */
template <typename SynapseType>
struct STDPAdditiveRule
{
    /**
     * @brief Type of the synapse linked with rule.
     */
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
     * @brief Steps when spikes on the synapse fired.
     */
    // cppcheck-suppress unusedStructMember
    std::vector<uint32_t> presynaptic_spike_times_;

    /**
     * @brief Steps when spikes on the axon fired.
     */
    // cppcheck-suppress unusedStructMember
    std::vector<uint32_t> postsynaptic_spike_times_;
};

}  // namespace knp::synapse_traits
