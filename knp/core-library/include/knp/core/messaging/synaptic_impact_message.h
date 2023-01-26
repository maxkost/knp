/**
 * @file spike_message.h
 * @brief Synaptic impact class.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include <map>
#include <vector>

#include "message_header.h"


namespace knp::core::messaging
{

struct SynapticImpact
{
    size_t connection_index_;
    double impact_value_;
    size_t presynaptic_neuron_index_;
    size_t postsynaptic_neuron_index_;
};

/**
 * @brief Synaptic impact message structure.
 */
struct SynapticImpactMessage
{
    MessageHeader header_;
    UID presynaptic_population_uid_;
    std::vector<SynapticImpact> impacts_;
};

}  // namespace knp::core::messaging
