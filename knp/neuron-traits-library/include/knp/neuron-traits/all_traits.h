/**
 * @file all_traits.h
 * @brief All neuron types traits list.
 * @author Artiom N.
 * @date 22.03.2023
 */

#pragma once

#include <knp/meta/stringify.h>

#include <boost/mp11.hpp>

#include "blifat.h"
#include "synaptic_resource_stdp_neurons.h"

/**
 * @brief Namespace for neuron traits.
 */
namespace knp::neuron_traits
{
// Comma-separated list of neurons.
#define ALL_NEURONS knp::neuron_traits::BLIFATNeuron, knp::neuron_traits::SynapticResourceSTDPBLIFATNeuron
#define BLIFAT_LIKE_NEURONS knp::neuron_traits::BLIFATNeuron, knp::neuron_traits::SynapticResourceSTDPBLIFATNeuron


/**
 * @brief List of neuron types.
 * @details To add a new neuron type to the list, define it in the ALL_NEURONS macro. For example,
 * @code{.cpp}
 * #define ALL_NEURONS knp::neuron_traits::BLIFATNeuron, knp::neuron_traits::IzhikevichNeuron
 * @endcode
 */
using AllNeurons = boost::mp11::mp_list<ALL_NEURONS>;


/**
 * @brief A tuple that contains string names of neuron types.
 */
constexpr auto neurons_names = KNP_MAKE_TUPLE(ALL_NEURONS);

}  // namespace knp::neuron_traits
