/**
 * @file spike_message.h
 * @brief Spike class.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include "message_header.h"


namespace knp::core::messaging
{

/**
 * @brief Spike message structure.
 */
struct SpikeMessage
{
    MessageHeader header_;
};

}  // namespace knp::core::messaging
