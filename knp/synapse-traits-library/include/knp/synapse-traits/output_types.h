/**
 * @file output_types.h
 * @brief Enumeration of different ways a synaptic impact may influence a neuron.
 * @author Vartenkov A.
 * @date 4.03.2023
 */

#pragma once

/**
 * @brief Synapse traits namespace.
 */
namespace knp::synapse_traits
{
// TODO: Improve descriptions.
/**
 * @brief Types of synapses. This parameter is used for projection-population interaction.
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
    DOPAMINE = 3
};

}  // namespace knp::synapse_traits
