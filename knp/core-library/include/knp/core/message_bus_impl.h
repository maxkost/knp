/**
 * @file message_bus_impl.h
 * @brief Message bus implementation interface class.
 * @author Vartenkov A.
 * @date 19.09.2023
 */
#pragma once
#include <knp/core/message_endpoint.h>

namespace knp::core
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
    virtual ~MessageBusImpl();

    /**
     * @brief Send a message between endpoints.
     */
    virtual bool step();

    /**
     * @brief Creates an endpoint that can be used for message exchange.
     * @return a new endpoint.
     */
    [[nodiscard]] virtual MessageEndpoint create_endpoint() = 0;

    /**
     * @brief Update if needed, to be called once before message routing.
     */
    virtual void update();
};
}  // namespace knp::core
