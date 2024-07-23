/**
 * @file message_bus_impl.h
 * @brief Message bus implementation interface class.
 * @author Vartenkov A.
 * @date 19.09.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
#pragma once
#include <knp/core/message_endpoint.h>

/**
 * @brief Namespace for implementations of message bus.
 */
namespace knp::core::messaging::impl
{
/**
 * @brief Base class for different message bus implementations.
 */
class MessageBusImpl
{
public:
    /**
     * @brief Default virtual destructor.
     */
    virtual ~MessageBusImpl() = default;

    /**
     * @brief Send a message between endpoints.
     * @return number of messages routed during this step.
     */
    virtual size_t step() = 0;

    /**
     * @brief Creates an endpoint that can be used for message exchange.
     * @return a new endpoint.
     */
    [[nodiscard]] virtual MessageEndpoint create_endpoint() = 0;

    /**
     * @brief Update if needed, to be called once before message routing.
     */
    virtual void update() {}
};
}  // namespace knp::core::messaging::impl
