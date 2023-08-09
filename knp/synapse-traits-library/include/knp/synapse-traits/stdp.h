/**
 * @file stdp.h
 * @brief STDP associations.
 * @author Artiom N.
 * @date 09.08.2023
 */

#pragma once

#include "delta.h"
#include "stdp_add_rule.h"
#include "type_traits.h"

/**
 * @brief Synapse traits namespace.
 */
namespace knp::synapse_traits
{

/**
 * @brief Delta synapse with STDP additive rule type.
 */
using AdditiveSTDPDeltaSynapse = STDPAdditiveRule<DeltaSynapse>;

/**
 * @brief Delta synapse with STDP add rule.
 */
template <>
struct synapse_parameters<AdditiveSTDPDeltaSynapse>
{
    synapse_parameters() : weight_(0.0f) {}

    explicit synapse_parameters(float weight) : weight_(weight) {}

    /**
     * @brief Synapse parameters.
     */
    synapse_parameters<DeltaSynapse> synapse_;

    /**
     * @brief STDP weight to add.
     */
    float weight_;
};

}  // namespace knp::synapse_traits
