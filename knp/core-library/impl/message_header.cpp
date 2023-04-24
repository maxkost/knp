/**
 * @file message_header.cpp
 * @brief Message header I/O operators implementation.
 * @author Vartenkov A.
 * @date 23.03.2023
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
    stream << header.sender_uid_ << header.send_time_;
    return stream;
}

}  // namespace knp::core::messaging
