/**
 * @file connectors.h
 * @brief Population connector.
 * @author Artiom N.
 * @date 21.10.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
#pragma once

#include <knp/core/population.h>
#include <knp/core/projection.h>

#include <optional>

#include "creators.h"
#include "synapse_parameters_generators.h"

/**
 * @brief Projection namespace.
 */
namespace knp::framework::projection
{

/**
 * @brief Connect source and destination populations.
 * @details Make all to all connection between two populations.
 * @tparam SourceNeuronType type of the source population neuron.
 * @tparam DestinationNeuronType type of the destination population neuron.
 * @tparam SynapseType new projection synapse type.
 * @param src presynaptic population.
 * @param dst postsynaptic population.
 * @param syn_gen synapse parameters generator.
 * @return new projection between two populations.
 */
template <typename SynapseType, typename SourceNeuronType, typename DestinationNeuronType>
[[nodiscard]] typename core::Projection<SynapseType> connect_populations(
    const core::Population<SourceNeuronType> &src, const core::Population<DestinationNeuronType> &dst,
    typename parameters_generators::SynGen2ParamsType<SynapseType> syn_gen =
        parameters_generators::default_synapse_gen<SynapseType>)
{
    return creators::all_to_all<SynapseType>(src.get_uid(), dst.get_uid(), src.size(), dst.size(), syn_gen);
}


/**
 * @brief Connect source and destination populations.
 * @details Make arbitrary projection.
 * @tparam SourceNeuronType type of the source population neuron.
 * @tparam DestinationNeuronType type of the destination population neuron.
 * @tparam SynapseType new projection synapse type.
 * @param src presynaptic population.
 * @param dst postsynaptic population.
 * @param syn_gen synapse generator.
 * @param num_iterations projection generator call iterations.
 * @return new projection between two populations.
 */
template <typename SynapseType, typename SourceNeuronType, typename DestinationNeuronType>
[[nodiscard]] typename core::Projection<SynapseType> connect_populations(
    const core::Population<SourceNeuronType> &src, const core::Population<DestinationNeuronType> &dst,
    typename knp::core::Projection<SynapseType>::SynapseGenerator syn_gen, size_t num_iterations)
{
    return core::Projection<SynapseType>(src.get_uid(), dst.get_uid(), syn_gen, num_iterations);
}

}  // namespace knp::framework::projection
