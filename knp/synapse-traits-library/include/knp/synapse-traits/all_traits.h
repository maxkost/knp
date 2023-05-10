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


namespace knp::synapse_traits
{

// Comma-separated list of synapses.
#define ALL_SYNAPSES knp::synapse_traits::DeltaSynapse
/**
 * @brief List of synapse types.
 * @details To add a new synapse type to the list, define it in the ALL_SYNAPSES macro. For example,
 * @code{.cpp}
 * #define ALL_SYNAPSES knp::synapse_traits::DeltaSynapse, knp::synapse_traits::AdditiveSTDPSynapse
 * @endcode
 */
using AllSynapses = boost::mp11::mp_list<ALL_SYNAPSES>;

constexpr auto synapses_names = KNP_MAKE_TUPLE(ALL_SYNAPSES);

}  // namespace knp::synapse_traits
