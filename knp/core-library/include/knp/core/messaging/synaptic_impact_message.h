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
#include "pack_message.h"


namespace knp::core::messaging
{

/**
 * @brief A structure that contains the synaptic impact value and indexes of presynaptic and posynaptic neurons.
 * @details Synaptic impact changes parameters of neurons after the synapses state was calculated.
 */
struct SynapticImpact
{
    uint64_t connection_index_;
    float impact_value_;
    uint32_t presynaptic_neuron_index_;
    uint32_t postsynaptic_neuron_index_;

    bool operator==(const SynapticImpact &) const;
};


/**
 * @brief Strcuture of the synaptic impact message.
 */
struct SynapticImpactMessage
{
    MessageHeader header_;
    UID presynaptic_population_uid_;
    UID postsynaptic_population_uid_;
    knp::synapse_traits::OutputType output_type_;
    std::vector<SynapticImpact> impacts_;
};

typedef std::unordered_map<size_t, SynapticImpactMessage> SynapticMessageQueue;

std::istream &operator>>(std::istream &stream, SynapticImpact &impact);
std::ostream &operator<<(std::ostream &stream, const SynapticImpact &impact);
std::ostream &operator<<(std::ostream &stream, const SynapticImpactMessage &msg);
std::istream &operator>>(std::istream &stream, SynapticImpactMessage &msg);

std::vector<uint8_t> pack(const SynapticImpactMessage &msg);
template <>
SynapticImpactMessage unpack<SynapticImpactMessage>(std::vector<uint8_t> &buffer);

}  // namespace knp::core::messaging
