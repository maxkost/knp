/**
 * @file spike_message.h
 * @brief Synaptic impact class.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once
#include <knp/synapse-traits/output_types.h>

#include <map>
#include <vector>

#include "message_header.h"


namespace knp::core::messaging
{

/**
 * @brief A structure that contains the synaptic impact value and indexes of presynaptic and posynaptic neurons.
 * @details Synaptic impact changes parameters of neurons after the synapses state was calculated.
 */
struct SynapticImpact
{
    size_t connection_index_;
    double impact_value_;
    size_t presynaptic_neuron_index_;
    size_t postsynaptic_neuron_index_;
};


/**
 * @brief Strcuture of the synaptic impact message.
 */
struct SynapticImpactMessage
{
    MessageHeader header_;
    UID postsynaptic_population_uid_;
    UID presynaptic_population_uid_;
    knp::synapse_traits::OutputType output_type;
    std::vector<SynapticImpact> impacts_;
};

}  // namespace knp::core::messaging
