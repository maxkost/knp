/**
 * @file message_bus.h
 * @brief Message bus interface.
 * @author Artiom N.
 * @date 23.01.2023
 */

#pragma once

#include <knp/core/messaging/message_envelope.h>
#include <knp/core/messaging/messaging.h>
#include <knp/core/subscription.h>
#include <knp/core/uid.h>

#include <any>
#include <functional>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <variant>
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
    using AllSubscriptions = boost::mp11::mp_transform<Subscription, messaging::AllMessages>;

    using SubscriptionVariant = boost::mp11::mp_rename<AllSubscriptions, std::variant>;

public:
    static UID get_receiver_uid(const SubscriptionVariant &subscription);
    static std::pair<size_t, UID> get_subscription_key(const SubscriptionVariant &subscription);

    template <typename Variant, typename Type>
    static constexpr size_t get_type_index = boost::mp11::mp_find<Variant, Type>::value;

public:
    MessageEndpoint(MessageEndpoint &&endpoint);
    MessageEndpoint &operator=(MessageEndpoint &&endpoint) = default;
    MessageEndpoint &operator=(MessageEndpoint &endpoint) = delete;
    virtual ~MessageEndpoint();

public:
    /**
     * @brief Add subscription to parameters or update an existing one
     * @tparam MessageType message type the receiver subscribes to
     * @param receiver receiver UID
     * @param senders a vector of sender UIDs
     * @return the number of senders added
     */
    template <typename MessageType>
    Subscription<MessageType> &subscribe(const UID &receiver, const std::vector<UID> &senders);

    /**
     * @brief Unsubscribe from messages of a certain type
     * @param receiver receiver UID.
     */
    template <typename MessageType>
    void unsubscribe(const UID &receiver);

    /**
     * @brief Remove all subscriptions with given reciever id
     * @param receiver UID of the receiver
     */
    void remove_receiver(const UID &receiver);

    /**
     * @brief Send a message to the message bus.
     * @param message message to send.
     */
    void send_message(const knp::core::messaging::MessageVariant &message);

    /**
     * @brief Receive a message from the message bus.
     * @return true if nonempty message was received, false otherwise.
     */
    bool receive_message();

    /**
     * @brief Read messages from subscription as a vector, clear them all after reading
     * @param receiver_uid UID of the receiving object
     */
    template <class MessageType>
    std::vector<MessageType> unload_messages(const knp::core::UID &receiver_uid)
    {
        constexpr size_t index = get_type_index<knp::core::messaging::MessageVariant, MessageType>;
        auto iter = subscriptions_.find(std::make_pair(index, receiver_uid));
        if (iter == subscriptions_.end()) return {};
        Subscription<MessageType> &subscription = std::get<index>(iter->second);
        auto result = std::move(subscription.get_messages());
        subscription.clear_messages();
        return result;
    }

    /**
     * @brief Receive messages in the cycle.
     */
    void receive_all_messages();

public:
    using SubscriptionContainer = std::map<std::pair<size_t, UID>, SubscriptionVariant>;

protected:
    /// Message endpoint implementation.
    class MessageEndpointImpl;
    std::unique_ptr<MessageEndpointImpl> impl_;

protected:
    MessageEndpoint() = default;

private:
    SubscriptionContainer subscriptions_;
};

}  // namespace knp::core
