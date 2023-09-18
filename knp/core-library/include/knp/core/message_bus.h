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
 * @brief Base class for different message bus implementations.
 */
class MessageBusImpl
{
public:
    /**
     * @brief Send a message between endpoints.
     */
    virtual bool step() = 0;

    /**
     * @brief Creates an endpoint that can be used for message exchange.
     * @return a new endpoint.
     */
    [[nodiscard]] virtual MessageEndpoint create_endpoint() = 0;

    /**
     * @brief Default virtual destructor.
     */
    virtual ~MessageBusImpl() = default;
};


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
     * @brief Message bus constructor with a specialized implementation.
     */
    explicit MessageBus(std::unique_ptr<MessageBusImpl> &&impl);

    /**
     * @brief Message bus destructor.
     */
    ~MessageBus() = default;

public:
    /**
     * @brief Create a new endpoint that sends and receives messages through the message bus.
     * @return new endpoint.
     * @see MessageEndpoint.
     */
    [[nodiscard]] MessageEndpoint create_endpoint();

    /**
     * @brief Route a single message.
     * @return `true` if a message was routed, `false` if no message was routed.
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
    std::unique_ptr<MessageBusImpl> impl_;
};

}  // namespace knp::core
