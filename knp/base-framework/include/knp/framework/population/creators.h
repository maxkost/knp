/**
 * @file creators.h
 * @brief Population creators.
 * @date 08.08.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
#pragma once

#include <knp/core/population.h>

#include <cinttypes>
#include <optional>
#include <random>

#include "neuron_parameters_generators.h"


/**
 * @brief Namespace for framework population routines.
 */
namespace knp::framework::population
{

/**
 * @brief Namespace for population creators.
 */
namespace creators
{
/**
 * @brief Generate a population from a container.
 * @param container container with neuron parameters.
 * @tparam NeuronType neuron type.
 * @tparam Container container type.
 * @return population.
 */
template <typename NeuronType, template <typename...> class Container>
[[nodiscard]] typename core::Population<NeuronType> from_container(
    const Container<typename core::Population<NeuronType>::NeuronParameters>& container)
{
    return core::Population<NeuronType>(neurons_generators::from_container<NeuronType>(container), container.size());
}


/**
 * @brief Generate a random population.
 * @details This generator uses MT19937 RNG with uniform integer distribution.
 * @warning Neurons parameters are absolutely random: generator doesn't pay attention to the limits.
 * @param neuron_count number of neurons in a population.
 * @tparam NeuronType type of neuron parameters.
 * @return population.
 */
template <typename NeuronType>
[[nodiscard]] typename core::Population<NeuronType> make_random(size_t neuron_count)
{
    return core::Population<NeuronType>(neurons_generators::MakeRandom<NeuronType>(), neuron_count);
}


/**
 * @brief Generate a population with default neuron parameter values.
 * @param neuron_count number of neurons in a population.
 * @tparam NeuronType type of neuron parameters.
 * @return population.
 */
template <typename NeuronType>
[[nodiscard]] typename core::Population<NeuronType> make_default(size_t neuron_count)
{
    return core::Population<NeuronType>(neurons_generators::make_default<NeuronType>(), neuron_count);
}


/**
 * @brief Generate a population with clone of neuron parameter values.
 * @param neuron_count number of neurons in a population.
 * @param source_neuron parameters source.
 * @tparam NeuronType type of neuron parameters.
 * @return population.
 */
template <typename NeuronType>
[[nodiscard]] typename core::Population<NeuronType> make_clone(
    size_t neuron_count, const typename core::Population<NeuronType>::NeuronParameters& source_neuron)
{
    return core::Population<NeuronType>(neurons_generators::make_clone<NeuronType>(source_neuron), neuron_count);
}

}  // namespace creators

}  // namespace knp::framework::population
