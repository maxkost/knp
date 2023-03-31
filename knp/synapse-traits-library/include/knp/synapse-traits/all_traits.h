/**
 * @file delta.h
 * @brief Delta synapse types traits.
 * @author Artiom N.
 * @date 22.03.2023
 */

#pragma once

#include <boost/mp11.hpp>

#include "delta.h"


namespace knp::synapse_traits
{

// Comma-separated synapses list.
#define ALL_SYNAPSES knp::synapse_traits::DeltaSynapse
using AllSynapses = boost::mp11::mp_list<ALL_SYNAPSES>;

}  // namespace knp::synapse_traits
