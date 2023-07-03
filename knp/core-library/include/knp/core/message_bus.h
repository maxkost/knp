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

/**
 * @brief Core library namespace.
 */
namespace knp::core
{

/**
 * @brief The MessageBus class is a definition of an interface to a message bus.
 */
class MessageBus
{
public:
    /**
     * @brief Default message bus constructor.
     */
    MessageBus();

    /**
     * @brief Message bus destructor.
     */
    ~MessageBus();

public:
    /**
     * @brief Create a new endpoint that sends and receives messages through the message bus.
     * @return new endpoint.
     * @see MessageEndpoint.
     */
    [[nodiscard]] MessageEndpoint create_endpoint();

    /**
     * @brief Route a single message.
     * @return true if a message was routed, false if no message was routed.
     */
    bool step();

    /**
     * @brief Route messages.
     * @return number of messages routed.
     */
    size_t route_messages();

private:
    /**
     * @brief Message bus implementation.
     */
    class MessageBusImpl;

    std::unique_ptr<MessageBusImpl> impl_;
};

}  // namespace knp::core
