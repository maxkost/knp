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


/**
 * @brief Messaging namespace.
 */
namespace knp::core::messaging
{

/**
 * @brief Structure of the spike message.
 */
struct SpikeMessage
{
    /**
     * @brief Message header.
     */
    MessageHeader header_;

    /**
     * @brief Indexes of the recently spiked neurons.
     */
    std::vector<uint32_t> neuron_indexes_;
};


std::ostream &operator<<(std::ostream &stream, const SpikeMessage &msg);
std::istream &operator>>(std::istream &stream, SpikeMessage &msg);

}  // namespace knp::core::messaging
