/**
 * @file spike_message.h
 * @brief Synaptic impact class.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once
#include <knp/synapse-traits/output_types.h>

#include <iostream>
#include <limits>
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

    /**
     * @brief Compare synaptic impact messages.
     */
    bool operator==(const SynapticImpact &) const;
};


/**
 * @brief Structure of the synaptic impact message.
 */
struct SynapticImpactMessage
{
    /**
     * @brief Message header.
     */
    MessageHeader header_;

    /**
     * @brief UID of the population that sends spikes to the projection.
     */
    UID presynaptic_population_uid_;

    /**
     * @brief UID of the population that receives impacts from the projection.
     */
    UID postsynaptic_population_uid_;

    // TODO: Try to remove this when fixing main; this parameter is too specific to be a part of a general message.
    /**
     * @brief `true` if the signal is from a projection without plasticity. 
     * @details The parameter is used in training.
     */
    bool is_forcing_ = false;

    /**
     * @brief Impact values.
     */
    std::vector<SynapticImpact> impacts_;
};

/**
 * @brief Synaptic messages that the projection will send in the future depending on the network execution step number.
 */
typedef std::unordered_map<uint64_t, SynapticImpactMessage> SynapticMessageQueue;

/**
 * @brief Get synaptic impact from an input stream.
 * @param stream input stream.
 * @param impact synaptic impact to get from the input stream.
 */
std::istream &operator>>(std::istream &stream, SynapticImpact &impact);

/**
 * @brief Send synaptic impact to an output stream.
 * @param stream output stream.
 * @param impact synaptic impact to send to the output stream.
 */
std::ostream &operator<<(std::ostream &stream, const SynapticImpact &impact);

/**
 * @brief Send synaptic impact message to an output stream.
 * @param stream output stream.
 * @param msg synaptic impact message to send to the output stream.
 */
std::ostream &operator<<(std::ostream &stream, const SynapticImpactMessage &msg);

/**
 * @brief Get synaptic impact message from an input stream.
 * @param stream input stream.
 * @param msg synaptic impact message to get from the input stream.
 */
std::istream &operator>>(std::istream &stream, SynapticImpactMessage &msg);

}  // namespace knp::core::messaging
