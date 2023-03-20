/**
 * @file message_bus.h
 * @brief Message bus interface.
 * @author Artiom N.
 * @date 23.01.2023
 */

#pragma once

#include <knp/core/messaging.h>
#include <knp/core/uid.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <boost/mp11.hpp>


namespace knp::core
{

/**
 * @brief The MessageEndpoint class is a definition of message endpoints.
 * @details You can use message endpoints to receive or send messages.
 */
class MessageEndpoint
{
public:
    using SupportedMessages =
        boost::mp11::mp_list<knp::core::messaging::SpikeMessage, knp::core::messaging::SynapticImpactMessage>;

public:
    MessageEndpoint(MessageEndpoint &&endpoint);
    MessageEndpoint &operator=(MessageEndpoint &&endpoint) = default;
    virtual ~MessageEndpoint();

public:
    /**
     * @brief Subscribe to messages.
     * @param publisher_uid UID of the object which messages to receive.
     * @param callback a Callable to use after receiving the message.
     * @tparam MessageType type of messages to receive.
     * @return subscription UID.
     */
    template <typename MessageType>
    UID subscribe(const UID &publisher_uid, std::function<void(const MessageType &)> callback);

    template <typename MessageType>
    UID subscribe(const UID &receiver, const std::vector<UID> &senders);

    /**
     * @brief Unsubscribe from messages.
     * @param subscription_uid subscription UID.
     */
    void unsubscribe(const UID &subscription_uid);

    /**
     * @brief Send a message to the message bus.
     * @param message message to send.
     */
    template <typename MessageType>
    void send_message(const MessageType &message);

    /**
     * @brief Receive a message from the message bus.
     * @param message message to receive.
     */
    template <typename MessageType>
    MessageType receive_message();

protected:
    explicit MessageEndpoint(void *context, const std::string &sub_addr, const std::string &pub_addr);

private:
    MessageEndpoint() = delete;

private:
    /// Message endpoint implementation.
    class MessageEndpointImpl;

    std::unique_ptr<MessageEndpointImpl> impl_;
};

}  // namespace knp::core
