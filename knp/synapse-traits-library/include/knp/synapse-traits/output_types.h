/**
 * @file output_types.h
 * @brief Enumeration of different ways that a synaptic impact may influence a neuron.
 * @author Vartenkov A.
 * @date 4.03.2023
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
 * @brief Types of synapses. This parameter is used for projection-population interaction.
 * @todo Improve descriptions.
 * Maybe split this enum.
 */
enum class OutputType
{
    /**
     * @brief Excitatory synapse type.
     */
    EXCITATORY = 0,

    /**
     * @brief Inhibitory by current synapse type.
     */
    INHIBITORY_CURRENT = 1,

    /**
     * @brief Inhibitory by conductance synapse type.
     */
    INHIBITORY_CONDUCTANCE = 2,

    /**
     * @brief Dopamine synapse type.
     */
    DOPAMINE = 3,

    /**
     * @brief Neuron-blocking synapse type.
     */
    BLOCKING = 4
};

}  // namespace knp::synapse_traits
