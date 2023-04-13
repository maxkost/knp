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


namespace knp::core::messaging
{

/**
 * @brief Common header for messages.
 */
struct MessageHeader
{
    knp::core::UID sender_uid_;
    uint64_t send_time_;
};

std::istream &operator>>(std::istream &stream, MessageHeader &header);
std::ostream &operator<<(std::ostream &stream, const MessageHeader &header);

template <typename T>
T unpack(std::vector<uint8_t> &buffer);

}  // namespace knp::core::messaging
