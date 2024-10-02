/**
 * @file message_header.cpp
 * @brief Implementation of message header I/O operators.
 * @author Vartenkov A.
 * @date 23.03.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/core/messaging/message_header.h>

namespace knp::core::messaging
{

std::istream &operator>>(std::istream &stream, MessageHeader &header)
{
    stream >> header.sender_uid_ >> header.send_time_;
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const MessageHeader &header)
{
    stream << header.sender_uid_ << " " << header.send_time_;
    return stream;
}

}  // namespace knp::core::messaging
