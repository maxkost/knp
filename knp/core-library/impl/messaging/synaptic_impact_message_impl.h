/**
 * @file synaptic_impact_message_impl.h
 * @brief Synaptic impact message I/O operators implementation.
 * @author Artiom N.
 * @date 23.03.2023
 */

#pragma once

#include <knp/core/messaging/synaptic_impact_message.h>

#include <knp_gen_headers/synaptic_impact_message_generated.h>


/**
 * @brief Messaging namespace.
 */
namespace knp::core::messaging
{
::flatbuffers::uoffset_t pack_internal(::flatbuffers::FlatBufferBuilder &builder, const SynapticImpactMessage &msg);
SynapticImpactMessage unpack(const marshal::SynapticImpactMessage *s_msg);
}  // namespace knp::core::messaging
