/**
 * @file neuron_parameters_generators.h
 * @brief Generators of the neurons.
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
 * @brief Namespace for framework population routines.
 */
namespace knp::framework::population
{

namespace neurons_generators
{

/**
 * @brief Generate a population from a container.
 * @param container container with neuron parameters.
 * @tparam NeuronType neuron type.
 * @tparam Container container type.
 * @return neuron parameters generator.
 */
template <typename NeuronType, template <typename...> class Container>
[[nodiscard]] typename core::Population<NeuronType>::NeuronGenerator from_container(
    const Container<typename core::Population<NeuronType>::NeuronParameters>& container)
{
    return [&container](size_t index) -> std::optional<typename core::Population<NeuronType>::NeuronParameters>
    { return container[index]; };
}


/**
 * @brief Generate a random population.
 * @details This generator uses MT19937 RNG with uniform integer distribution.
 * @warning Neurons parameters are absolutely random: generator doesn't pay attention to the limits.
 * @tparam NeuronType type of neuron parameters.
 */
template <typename NeuronType>
class MakeRandom
{
public:
    /**
     * @brief Constructor.
     */
    MakeRandom() : mt_(std::random_device()()), dist_(0, 255) {}

    /**
     * @brief Call operator.
     * @param index neuron index.
     * @return optional neuron parameters.
     */
    [[nodiscard]] typename core::Population<NeuronType>::NeuronParameters operator()(size_t index)
    {
        typename core::Population<NeuronType>::NeuronParameters params;
        for (size_t i = 0; i < sizeof(params); ++i) reinterpret_cast<uint8_t*>(&params)[i] = dist_(mt_);
        return params;
    }

private:
    std::mt19937 mt_;
    std::uniform_int_distribution<int> dist_;
};


/**
 * @brief Generate a population with default neuron parameter values.
 * @tparam NeuronType type of neuron parameters.
 * @return neuron parameters generator.
 */
template <typename NeuronType>
[[nodiscard]] typename core::Population<NeuronType>::NeuronGenerator make_default()
{
    return [](size_t index) -> std::optional<typename core::Population<NeuronType>::NeuronParameters>
    { return typename core::Population<NeuronType>::NeuronParameters(); };
}


/**
 * @brief Generate a population with neuron parameter values copied from the source neuron.
 * @param source_neuron parameters source.
 * @tparam NeuronType type of neuron parameters.
 * @return neuron parameters generator.
 */
template <typename NeuronType>
[[nodiscard]] typename core::Population<NeuronType>::NeuronGenerator make_copy(
    const typename core::Population<NeuronType>::NeuronParameters source_neuron)
{
    return [source_neuron](size_t index) -> std::optional<typename core::Population<NeuronType>::NeuronParameters>
    { return source_neuron; };
}

}  // namespace neurons_generators
}  // namespace knp::framework::population
