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
 * @brief The MessageBus class is a definition of an interafce to a message bus.
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

private:
    /// Message bus implementation.
    class MessageBusImpl;

    std::unique_ptr<MessageBusImpl> impl_;
};

}  // namespace knp::core
