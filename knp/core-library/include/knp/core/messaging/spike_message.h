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
 * @brief Spike index type in the form of a 32-bit unsigned integer.
 */
using SpikeIndex = uint32_t;

/**
 * @brief List of spike indexes.
 */
using SpikeData = std::vector<SpikeIndex>;


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
    SpikeData neuron_indexes_;
};


/**
 * @brief Send spike message to an output stream.
 * @param stream output stream.
 * @param msg spike message to send to the output stream.
 */
std::ostream &operator<<(std::ostream &stream, const SpikeMessage &msg);

/**
 * @brief Get spike message from an input stream.
 * @param stream input stream.
 * @param msg spike message to get from the input stream.
 */
std::istream &operator>>(std::istream &stream, SpikeMessage &msg);

}  // namespace knp::core::messaging
