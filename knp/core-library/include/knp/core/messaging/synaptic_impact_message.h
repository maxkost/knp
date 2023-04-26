/**
 * @file spike_message.h
 * @brief Synaptic impact class.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once
#include <knp/synapse-traits/output_types.h>

#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

#include "message_header.h"


/**
 * @brief Messaging namespace.
 */
namespace knp::core::messaging
{

/**
 * @brief Structure that contains the synaptic impact value and indexes of presynaptic and posynaptic neurons.
 * @details Synaptic impact changes parameters of neurons after the synapses state is calculated.
 */
struct SynapticImpact
{
    /**
     * @brief Index of the population synapse.
     */
    uint64_t connection_index_;

    /**
     * @brief Value used to change neuron membrane potential.
     */
    float impact_value_;

    /**
     * @brief Synapse type that might define the value role inside the neuron function.
     */
    knp::synapse_traits::OutputType synapse_type_;

    /**
     * @brief Index of the presynaptic neuron connected to the synapse.
     */
    uint32_t presynaptic_neuron_index_;

    /**
     * @brief Index of the postsynaptic neuron connected to the synapse.
     */
    uint32_t postsynaptic_neuron_index_;

    bool operator==(const SynapticImpact &) const;
};


/**
 * @brief Structure of the synaptic impact message.
 */
struct SynapticImpactMessage
{
    MessageHeader header_;

    /**
     * @brief UID of the population that sends spikes to the projection.
     */
    UID presynaptic_population_uid_;

    /**
     * @brief UID of the population that receives impacts from the projection.
     */
    UID postsynaptic_population_uid_;

    /**
     * @brief Impact values.
     */
    std::vector<SynapticImpact> impacts_;
};

typedef std::unordered_map<size_t, SynapticImpactMessage> SynapticMessageQueue;

std::istream &operator>>(std::istream &stream, SynapticImpact &impact);
std::ostream &operator<<(std::ostream &stream, const SynapticImpact &impact);
std::ostream &operator<<(std::ostream &stream, const SynapticImpactMessage &msg);
std::istream &operator>>(std::istream &stream, SynapticImpactMessage &msg);

}  // namespace knp::core::messaging
