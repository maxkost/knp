/**
 * @file message_bus.h
 * @brief Message bus interface.
 * @author Artiom N.
 * @date 23.01.2023
 */

#pragma once

#include <knp/core/message_endpoint.h>

#include <functional>
#include <memory>


namespace knp::core
{

/**
 * @brief The MessageBus class defines an interafce to the message bus.
 */
class MessageBus
{
public:
    MessageBus();
    ~MessageBus();

public:
    /**
     * @brief Get endpoint that sends and receives messages through the message bus.
     * @see MessageEndpoint.
     * @return endpoint that uses the message bus.
     */
    [[nodiscard]] MessageEndpoint get_endpoint();

    /**
     * @brief Receive and send all messages in the message queue
     * @return number of messages routed
     * @throws zmq::error_t on standard zmq errors
     * @throws std::logic_error on not being able to write message to socket
     */
    size_t route_messages();

    /**
     * @brief Receive message from an endpoint, send it to an endpoint
     * @return true if message successfully received, false if no message
     */
    bool step();

private:
    /// Message bus implementation.
    class MessageBusImpl;

    std::unique_ptr<MessageBusImpl> impl_;
};

}  // namespace knp::core
