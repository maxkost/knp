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

using AllNeurons = boost::mp11::mp_list<BLIFATNeuron>;

}  // namespace knp::neuron_traits
