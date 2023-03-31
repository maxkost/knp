/**
 * @file message_header.h
 * @brief Message headers class.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include <knp/core/uid.h>

#include <iostream>


namespace knp::core::messaging
{

/**
 * @brief Common header for messages.
 */
struct MessageHeader
{
    knp::core::UID sender_uid_;
    time_t send_time_;
};

std::istream &operator>>(std::istream &stream, MessageHeader &header);
std::ostream &operator<<(std::ostream &stream, const MessageHeader &header);

}  // namespace knp::core::messaging
