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
#include <optional>
#include <random>


/**
 * @brief Framework population routines namespace.
 */
namespace knp::framework::population
{

#if (!defined(_MSC_VER))
// Can't be compiled under MSVC.

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
    return core::Population<NeuronType>(
        [&container](size_t index) -> std::optional<typename core::Population<NeuronType>::NeuronParameters>
        { return container[index]; },
        container.size());
}
#endif


/**
 * @brief Generate a random population.
 * @param neuron_count number of neurons in a population.
 * @tparam NeuronType type of neuron parameters.
 * @return population.
 */
template <typename NeuronType>
[[nodiscard]] typename core::Population<NeuronType> make_random(size_t neuron_count)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0, 255);

    return core::Population<NeuronType>(
        [&dist, &mt](size_t index) -> std::optional<typename core::Population<NeuronType>::NeuronParameters>
        {
            typename core::Population<NeuronType>::NeuronParameters params;
            for (size_t i = 0; i < sizeof(params); ++i)
                reinterpret_cast<uint8_t*>(&params)[i] = static_cast<uint8_t>(dist(mt));

            return params;
        },
        neuron_count);
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
    return core::Population<NeuronType>(
        [](size_t index) -> std::optional<typename core::Population<NeuronType>::NeuronParameters>
        { return typename core::Population<NeuronType>::NeuronParameters(); },
        neuron_count);
}
}  // namespace knp::framework::population
