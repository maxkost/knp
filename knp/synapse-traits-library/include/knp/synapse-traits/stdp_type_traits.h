/**
 * @file stdp_type_traits.h
 * @brief STDP associations and type traits.
 * @author Artiom N.
 * @date 19.09.2023
 */

#pragma once

#include <set>

#include "delta.h"
#include "stdp_add_rule.h"
#include "stdp_synaptic_resource_rule.h"

/**
 * @brief Synapse traits namespace.
 */
namespace knp::synapse_traits
{

/**
 * @brief Delta synapse with STDP additive rule type.
 */
using AdditiveSTDPDeltaSynapse = STDP<STDPAdditiveRule, DeltaSynapse>;


/**
 * @brief Delta synapse with STDP synaptic resource rule type.
 */
using SynapticResourceSTDPDeltaSynapse = STDP<STDPSynapticResourceRule, DeltaSynapse>;


}  // namespace knp::synapse_traits
