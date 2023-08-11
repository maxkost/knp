/**
 * @file stdp.h
 * @brief STDP associations.
 * @author Artiom N.
 * @date 09.08.2023
 */

#pragma once

#include <set>

#include "delta.h"
#include "stdp_add_rule.h"
#include "type_traits.h"

/**
 * @brief Synapse traits namespace.
 */
namespace knp::synapse_traits
{

/**
 * @brief Common template for the all STDP implementations.
 */
template <template <typename> typename Rule, typename Synapse>
struct STDP;


/**
 * @brief Synapse with STDP rule.
 * @details STDP rule is a template, because sometimes parameters can be different in the
 *  different rule/synapse combinations. This allows make that.
 * @code
   template<>
   struct STDPAdditiveRule<DeltaSynapse>
   {
        using LinkedSynapseType = DeltaSynapse;

        float weight1_;
        float weight2_;
    };
 * @endcode
 */
template <template <typename> typename Rule, typename Synapse>
struct synapse_parameters<STDP<Rule, Synapse>>
{
    /**
     * @brief Specific STDP rule type.
     */
    using RuleType = Rule<Synapse>;
    /**
     * @brief Synapse model type.
     */
    using SynapseType = Synapse;

    /**
     * @brief STDP rule parameters.
     */
    RuleType rule_;
    /**
     * @brief Synapse model parameters.
     */
    synapse_parameters<SynapseType> synapse_;
};


/**
 * @brief Delta synapse with STDP additive rule type.
 */
using AdditiveSTDPDeltaSynapse = STDP<STDPAdditiveRule, DeltaSynapse>;

}  // namespace knp::synapse_traits
