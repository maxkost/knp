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
/**
 * @brief Type used to store a step number in the form of a 64-bit unsigned integer.
 */
using Step = uint64_t;

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
    Step send_time_;
};

/**
 * @brief Get message header from an input stream.
 * @param stream input stream.
 * @param header message header to get from the input stream.
 */
std::istream &operator>>(std::istream &stream, MessageHeader &header);

/**
 * @brief Send message header to an output stream.
 * @param stream output stream.
 * @param header message header to send to the output stream.
 */
std::ostream &operator<<(std::ostream &stream, const MessageHeader &header);

}  // namespace knp::core::messaging
