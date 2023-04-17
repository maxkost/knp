/**
 * @file spike_message.cpp
 * @brief Spike message implementation header.
 * @author Artiom N.
 * @date 13.04.2023
 */

#pragma once

#include <knp/core/messaging/spike_message.h>

#include <knp_gen_headers/spike_message_generated.h>


namespace knp::core::messaging
{
::flatbuffers::uoffset_t pack_internal(::flatbuffers::FlatBufferBuilder &builder, const SpikeMessage &msg);
SpikeMessage unpack(const marshal::SpikeMessage *s_msg);
}  // namespace knp::core::messaging
