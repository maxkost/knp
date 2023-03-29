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

// Comma-separated neurons list.
#define ALL_NEURONS knp::neuron_traits::BLIFATNeuron
using AllNeurons = boost::mp11::mp_list<ALL_NEURONS>;

}  // namespace knp::neuron_traits
