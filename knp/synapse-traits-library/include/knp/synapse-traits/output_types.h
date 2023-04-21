/**
 * @file output_types.h
 * @brief Enumeration of different ways a synaptic impact may influence a neuron.
 * @author Vartenkov A.
 * @date 4.03.2023
 */

#pragma once
namespace knp::synapse_traits
{
/**
 * @brief Types of synapses. This parameter is used for projection-population interaction.
 */
enum class OutputType
{
    EXCITATORY = 0,
    INHIBITORY_CURRENT = 1,
    INHIBITORY_CONDUCTANCE = 2,
    DOPAMINE = 3
};

}  // namespace knp::synapse_traits
