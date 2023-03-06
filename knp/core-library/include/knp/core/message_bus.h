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
 * @brief Message bus interface.
 */
class MessageBus
{
public:
    MessageBus();
    ~MessageBus();

public:
    /**
     * @brief endpoint returning method.
     * @see MessageEndpoint.
     * @return endpoint to communicate via the bus.
     */
    MessageEndpoint &&get_endpoint();

private:
    /// Message bus implementation.
    class MessageBusImpl;

    std::unique_ptr<MessageBusImpl> impl_;
};

}  // namespace knp::core
