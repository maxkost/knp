/**
 * @file all_traits.h
 * @brief Delta synapse type traits.
 * @author Artiom N.
 * @date 22.03.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/meta/stringify.h>

#include <boost/mp11.hpp>

#include "delta.h"
#include "stdp_type_traits.h"

/**
 * @brief Namespace for synapse traits.
 */
namespace knp::synapse_traits
{

/**
 * @brief Comma-separated list of synapse tags.
 */
#define ALL_SYNAPSES DeltaSynapse, AdditiveSTDPDeltaSynapse, SynapticResourceSTDPDeltaSynapse


/**
 * @brief List of synapse types.
 * @details To add a new synapse type to the list, define it in the ALL_SYNAPSES macro. For example,
 * @code{.cpp}
 * #define ALL_SYNAPSES DeltaSynapse, AdditiveSTDPDeltaSynapse
 * @endcode
 */
using AllSynapses = boost::mp11::mp_list<ALL_SYNAPSES>;


/**
 * @brief A tuple that contains string names of synapse types.
 */
constexpr auto synapses_names = KNP_MAKE_TUPLE(ALL_SYNAPSES);

}  // namespace knp::synapse_traits
