/**
 * @file synaptic_resource_stdp_neurons.h
 * @brief Synaptcic resource based STDP neurons parameters.
 * @author Artiom N.
 * @date 06.10.2023
 */

#pragma once

#include "blifat.h"
#include "synaptic_resource_stdp.h"

/**
 * @brief Namespace for neuron traits.
 */
namespace knp::neuron_traits
{

/**
 * @brief BLIFAT neuron with resource-based STDP support
 */
using SynapticResourceSTDPBLIFATNeuron = SynapticResourceSTDPNeuron<BLIFATNeuron>;


}  // namespace knp::neuron_traits
