/**
 * @file stdp_type_traits.h
 * @brief STDP-specific neuron types traits list.
 * @author A. Vartenkov.
 * @date 7.12.2023
 */

#pragma once

/**
 * @brief Namespace for neuron traits.
 */
namespace knp::neuron_traits
{
/**
 * @brief BLIFAT neuron with additional resource-based STDP parameters.
 */
using SynapticResourceSTDPBLIFATNeuron = SynapticResourceSTDPNeuron<BLIFATNeuron>;

}  // namespace knp::neuron_traits
