/**
 * @file message_bus.h
 * @brief Message bus interface.
 * @author Artiom N.
 * @date 23.01.2023
 */

#pragma once

#include <knp/core/uid.h>

#include <functional>
#include <memory>


namespace knp::core
{

/**
 * @brief Message endpoint.
 *
 * Need to subscribe to the messages or to send messages.
 */
class MessageEndpoint
{
public:
    /**
     * @brief message subscription method.
     * @param publisher_uid publisher which messages subscriber want to receive.
     * @param callback is a Callable will be used when the message received.
     * @tparam MessageType message class which subscriber want to receive.
     * @return subscription id.
     */
    template <typename MessageType>
    UID subscribe(const UID &publisher_uid, std::function<void(const MessageType &)> callback);

    /**
     * @brief message unsubscription method.
     * @param subscription_uid UID returned by subscribe method.
     */
    void unsubscribe(const UID &subscription_uid);

    /**
     * @brief Publish message to the bus.
     * @param message is a published message.
     */
    template <typename MessageType>
    void send_message(const MessageType &message);

    /**
     * @brief Publish message to the bus.
     * @param message is a published message. Object will be moved.
     */
    template <typename MessageType>
    void send_message(MessageType &&message);

private:
    /// Message endpoint implementation.
    class MessageEndpointImpl;

    std::unique_ptr<MessageEndpointImpl> impl_;
};

}  // namespace knp::core
