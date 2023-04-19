/**
 * @file all_traits.h
 * @brief All neuron types traits list.
 * @author Artiom N.
 * @date 22.03.2023
 */

#pragma once

#include <boost/mp11.hpp>

#include "blifat.h"


namespace knp::neuron_traits
{
// Comma-separated list of neurons.
#define ALL_NEURONS knp::neuron_traits::BLIFATNeuron
/**
* @brief List of neuron types.
* @details To add a new neuron type to the list, define it in the ALL_NEURONS macro. For example, 
* @code{.cpp} 
* #define ALL_NEURONS knp::neuron_traits::BLIFATNeuron, knp::neuron_traits::IzhikevichNeuron
* @endcode
*/
using AllNeurons = boost::mp11::mp_list<ALL_NEURONS>;

}  // namespace knp::neuron_traits
