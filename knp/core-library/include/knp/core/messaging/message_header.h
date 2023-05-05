/**
 * @file message_header.h
 * @brief Message headers class.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include <knp/core/uid.h>

#include <iostream>
#include <vector>


/**
 * @brief Messaging namespace.
 */
namespace knp::core::messaging
{

using TimeType = uint64_t;

/**
 * @brief Common header for messages.
 */
struct MessageHeader
{
    /**
     * @brief UID of the object that sent the message.
     */
    knp::core::UID sender_uid_;
    /**
     * @brief Index of the network execution step.
     */
    TimeType send_time_;
};

std::istream &operator>>(std::istream &stream, MessageHeader &header);
std::ostream &operator<<(std::ostream &stream, const MessageHeader &header);

}  // namespace knp::core::messaging
