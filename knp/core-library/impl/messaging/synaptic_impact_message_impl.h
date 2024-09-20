/**
 * @file synaptic_impact_message_impl.h
 * @brief Implementation od synaptic impact message I/O operators.
 * @author Artiom N.
 * @date 23.03.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/core/messaging/synaptic_impact_message.h>

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdocumentation"
#endif
#include <knp_gen_headers/synaptic_impact_message_generated.h>
#ifdef __clang__
#    pragma clang diagnostic pop
#endif

/**
 * @brief Messaging namespace.
 */
namespace knp::core::messaging
{
::flatbuffers::uoffset_t pack_internal(::flatbuffers::FlatBufferBuilder &builder, const SynapticImpactMessage &msg);
SynapticImpactMessage unpack(const marshal::SynapticImpactMessage *s_msg);
}  // namespace knp::core::messaging
