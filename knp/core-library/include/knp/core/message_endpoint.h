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
    /**
     * @brief type list for subscriptions to messages in the same order as AllMessages.
     */
    using AllSubscriptions = boost::mp11::mp_transform<Subscription, messaging::AllMessages>;

    /**
     * @brief subscription variant that can contain any possible subscription.
     */
    using SubscriptionVariant = boost::mp11::mp_rename<AllSubscriptions, std::variant>;

public:
    /**
     * Gets receiver UID from a subscription variant.
     * @param subscription subscription variant.
     * @return receiver UID.
     */
    static UID get_receiver_uid(const SubscriptionVariant &subscription);
    static std::pair<size_t, UID> get_subscription_key(const SubscriptionVariant &subscription);

    /**
     * @brief Find the index of a type in a variant.
     * @tparam Variant a variant of one or more types.
     * @tparam Type the type to search.
     */
    template <typename Variant, typename Type>
    static constexpr size_t get_type_index = boost::mp11::mp_find<Variant, Type>::value;

public:
    MessageEndpoint(MessageEndpoint &&endpoint);
    MessageEndpoint &operator=(MessageEndpoint &&endpoint) = default;
    MessageEndpoint &operator=(MessageEndpoint &endpoint) = delete;
    virtual ~MessageEndpoint();

public:
    /**
     * @brief Add a subscription for a receiver with given UID to messages of the specified type from senders with given
     * UIDs.
     * @note If the subscription for the specified receiver and message type already exists, update the list of senders
     * in the subscription.
     * @tparam MessageType type of messages to which the receiver subscribes via the subscription.
     * @param receiver receiver UID.
     * @param senders vector of sender UIDs.
     * @return number of senders added to the subscription.
     */
    template <typename MessageType>
    Subscription<MessageType> &subscribe(const UID &receiver, const std::vector<UID> &senders);

    /**
     * @brief Unsubscribe from messages of a specified type.
     * @tparam MessageType type of messages to which the receiver is subscribed.
     * @param receiver receiver UID.
     */
    template <typename MessageType>
    void unsubscribe(const UID &receiver);

    /**
     * @brief Remove all subscriptions for a receiver with given UID.
     * @param receiver receiver UID.
     */
    void remove_receiver(const UID &receiver);

    /**
     * @brief Send a message to the message bus.
     * @param message message to send.
     */
    void send_message(const knp::core::messaging::MessageVariant &message);

    /**
     * @brief Receive a message from the message bus.
     * @return true if a message was received
     *         false if no message.
     */
    bool receive_message();

    /**
     * @brief Read messages of the specified type received via subscription as a vector.
     * @note After reading the messages, the method clears them from the subscription.
     * @tparam MessageType type of messages to be unloaded.
     * @param receiver_uid receiver UID.
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
     * @brief Receive all messages that were sent to the endpoint.
     */
    void receive_all_messages();

public:
    /**
     * @brief Container type for subscriptions.
     */
    using SubscriptionContainer = std::map<std::pair<size_t, UID>, SubscriptionVariant>;

protected:
    /**
     * @brief Message endpoint implementation.
     */
    class MessageEndpointImpl;
    std::unique_ptr<MessageEndpointImpl> impl_;

protected:
    MessageEndpoint() = default;

private:
    /**
     * @brief Container that stores all the subscriptions for the current endpoint.
     */
    SubscriptionContainer subscriptions_;
};

}  // namespace knp::core
