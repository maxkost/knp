/**
 * @file stdp_type_traits.h
 * @brief STDP associations and type traits.
 * @kaspersky_support Artiom N.
 * @date 19.09.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <set>

#include "delta.h"
#include "stdp_add_rule.h"
#include "stdp_synaptic_resource_rule.h"

/**
 * @brief Namespace for synapse traits.
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
