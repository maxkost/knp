/**
 * @file stdp_type_traits.h
 * @brief List of STDP-specific neuron type traits.
 * @author A. Vartenkov.
 * @date 7.12.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
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
