/**
 * @file spike_message.h
 * @brief Spike class.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include <iostream>
#include <vector>

#include "message_header.h"


namespace knp::core::messaging
{

/**
 * @brief Structure of the spike message.
 */
struct SpikeMessage
{
    MessageHeader header_;
    std::vector<uint32_t> neuron_indexes_;
};


std::ostream &operator<<(std::ostream &stream, const SpikeMessage &msg);
std::istream &operator>>(std::istream &stream, SpikeMessage &msg);

std::vector<uint8_t> pack(const SpikeMessage &msg);

template <>
SpikeMessage unpack<SpikeMessage>(std::vector<uint8_t> &buffer);

}  // namespace knp::core::messaging
