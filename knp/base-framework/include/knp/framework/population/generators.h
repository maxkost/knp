/**
 * @file generators.h
 * @brief Population generators.
 * @date 08.08.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
#pragma once

#include <knp/core/population.h>

#include <cinttypes>
#include <random>


/**
 * @brief Framework population routines namespace.
 */
namespace knp::framework::population
{

/**
 * @brief Make population from the container.
 * @param container container with neurons parameters.
 * @tparam NeuronType neuron parameters type.
 * @tparam Container container type.
 * @return population.
 */
template <typename NeuronType, typename Container>
typename core::Population<NeuronType> from_container(const Container<typename NeuronType>& container)
{
    return core::Population<NeuronType>(
        [&container](size_t index) -> std::optional<NeuronParameters> { return container[index]; }, container.size());
}


/**
 * @brief Make random population.
 * @param neuron_count count of neurons in the population.
 * @tparam NeuronType neuron parameters type.
 * @return population.
 */
template <typename NeuronType>
typename core::Population<NeuronType> make_random(size_t neuron_count)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<std::uint8_t> dist(0, 255);

    return core::Population<NeuronType>(
        [&dist, &mt](size_t index) -> std::optional<NeuronParameters>
        {
            NeuronParameters params;
            for (size_t i = 0; i < sizeof(NeuronParameters); ++i) reinterpret_cast<uint8_t*>(&params)[i] = dist(mt);

            return params;
        },
        neuron_count);
}


/**
 * @brief Make population where neurons have default parameters values.
 * @param neuron_count count of neurons in the population.
 * @tparam NeuronType neuron parameters type.
 * @return population.
 */
template <typename NeuronType>
typename core::Population<NeuronType> make_default(size_t neuron_count)
{
    return core::Population<NeuronType>(
        [](size_t index) -> std::optional<NeuronParameters> { return NeuronParameters(); }, neuron_count);
}
}  // namespace knp::framework::population
