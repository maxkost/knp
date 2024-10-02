/**
 * @file type_traits.h
 * @brief Common synapse type traits.
 * @author Artiom N.
 * @date 26.01.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

/**
 * @brief Namespace for synapse traits.
 */
namespace knp::synapse_traits
{

/**
 * @brief Structure for synapse parameters.
 * @tparam SynapseType type of synapses.
 */
template <typename SynapseType>
struct synapse_parameters;


/**
 * @brief Structure for synapse default values.
 * @tparam SynapseType type of synapses.
 */
template <typename SynapseType>
struct default_values;


/**
 * @brief Structure for parameters shared between synapses.
 * @tparam SynapseType type of synapses.
 */
template <typename SynapseType>
struct shared_synapse_parameters
{
};
}  // namespace knp::synapse_traits
