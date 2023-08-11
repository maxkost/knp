/**
 * @file stdp_add_rule.h
 * @brief STDP with additive rule.
 * @author Artiom N.
 * @date 09.08.2023
 */

#pragma once

#include "type_traits.h"

/**
 * @brief Synapse traits namespace.
 */
namespace knp::synapse_traits
{

/**
 * @brief STDP additive rule parameters.
 */
template <typename SynapseType>
struct STDPAdditiveRule
{
    using LinkedSynapseType = SynapseType;

    /**
     * @brief STDP weight to add.
     */
    // cppcheck-suppress unusedStructMember
    float weight_;
};

}  // namespace knp::synapse_traits
