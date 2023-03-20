/**
 * @file spike_message.h
 * @brief Spike class.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

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

}  // namespace knp::core::messaging
