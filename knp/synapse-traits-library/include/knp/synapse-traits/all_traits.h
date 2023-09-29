/**
 * @file delta.h
 * @brief Delta synapse types traits.
 * @author Artiom N.
 * @date 22.03.2023
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

// Comma-separated list of synapses.
#define ALL_SYNAPSES                                                                  \
    knp::synapse_traits::DeltaSynapse, knp::synapse_traits::AdditiveSTDPDeltaSynapse, \
        knp::synapse_traits::SynapticResourceSTDPDeltaSynapse
/**
 * @brief List of synapse types.
 * @details To add a new synapse type to the list, define it in the ALL_SYNAPSES macro. For example,
 * @code{.cpp}
 * #define ALL_SYNAPSES knp::synapse_traits::DeltaSynapse, knp::synapse_traits::AdditiveSTDPDeltaSynapse
 * @endcode
 */
using AllSynapses = boost::mp11::mp_list<ALL_SYNAPSES>;

/**
 * @brief A tuple that contains string names of synapse types.
 */
constexpr auto synapses_names = KNP_MAKE_TUPLE(ALL_SYNAPSES);

}  // namespace knp::synapse_traits
