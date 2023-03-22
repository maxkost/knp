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
struct by_type
{
};
struct by_sender_uid
{
};
typedef std::variant<SpikeMessage, SynapticImpactMessage> MessageVariant;
}  // namespace knp::core::messaging
