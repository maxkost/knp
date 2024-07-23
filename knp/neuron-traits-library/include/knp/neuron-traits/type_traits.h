/**
 * @file type_traits.h
 * @brief Common neuron type traits.
 * @author Artiom N.
 * @date 26.01.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once
#include <type_traits>

/**
 * @brief Namespace for neuron traits.
 */
namespace knp::neuron_traits
{

/**
 * @brief Structure for neuron parameters.
 * @tparam NeuronType type of neurons.
 */
template <typename NeuronType>
struct neuron_parameters;

/**
 * @brief Structure for neuron default values.
 * @tparam NeuronType type of neurons.
 */
template <typename NeuronType>
struct default_values;


}  // namespace knp::neuron_traits
