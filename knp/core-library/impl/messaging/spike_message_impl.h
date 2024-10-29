/**
 * @file spike_message.cpp
 * @brief Spike message implementation header.
 * @kaspersky_support Artiom N.
 * @date 13.04.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/core/messaging/spike_message.h>

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdocumentation"
#endif
#include <knp_gen_headers/spike_message_generated.h>
#ifdef __clang__
#    pragma clang diagnostic pop
#endif

/**
 * @brief Messaging namespace.
 */
namespace knp::core::messaging
{
::flatbuffers::uoffset_t pack_internal(::flatbuffers::FlatBufferBuilder &builder, const SpikeMessage &msg);
SpikeMessage unpack(const marshal::SpikeMessage *s_msg);
}  // namespace knp::core::messaging
