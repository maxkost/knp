/**
 * @file messaging.h
 * @brief Messaging common header.
 * @author Artiom N.
 * @date 02.03.2023
 */

#pragma once

#include <knp/core/messaging/spike_message.h>
#include <knp/core/messaging/synaptic_impact_message.h>


namespace knp::core::messaging
{

typedef std::variant<SpikeMessage, SynapticImpactMessage> MessageVariant;

std::istream &operator>>(std::istream &stream, MessageHeader &header);
std::ostream &operator<<(std::ostream &stream, const MessageHeader &header);
std::istream &operator>>(std::istream &stream, SynapticImpact &impact);
std::ostream &operator<<(std::ostream &stream, const SynapticImpact &impact);
std::ostream &operator<<(std::ostream &stream, const SynapticImpactMessage &msg);
std::istream &operator>>(std::istream &stream, SynapticImpactMessage &msg);
std::ostream &operator<<(std::ostream &stream, const SpikeMessage &msg);
std::istream &operator>>(std::istream &stream, SpikeMessage &msg);

}  // namespace knp::core::messaging
