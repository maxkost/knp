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
 * @brief Structure for STDP add rule parameters.
 */
template <typename SynapseType>
struct STDPAdditiveRule;

}  // namespace knp::synapse_traits
