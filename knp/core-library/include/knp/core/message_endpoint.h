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
     * @brief List of subscription types based on message types specified in `messaging::AllMessages`.
     */
    using AllSubscriptions = boost::mp11::mp_transform<Subscription, messaging::AllMessages>;

    /**
     * @brief Subscription variant that contains any subscription type specified in `AllSubscriptions`.
     * @details `SubscriptionVariant` takes the value of `std::variant<SubscriptionType_1,..., SubscriptionType_n>`,
     * where `SubscriptionType_[1..n]` is the subscription type specified in `AllSubscriptions`. \n For example, if
     * `AllSubscriptions` containes SpikeMessage and SynapticImpactMessage types, then `SubscriptionVariant =
     * std::variant<SpikeMessage, SynapticImpactMessage>`. \n `SubscriptionVariant` retains the same order of message
     * types as defined in `AllSubscriptions`.
     * @see ALL_MESSAGES.
     */

    using SubscriptionVariant = boost::mp11::mp_rename<AllSubscriptions, std::variant>;

public:
    /**
     * @brief Get receiver UID from a subscription variant.
     * @param subscription subscription variant.
     * @return receiver UID.
     */
    static UID get_receiver_uid(const SubscriptionVariant &subscription);
    static std::pair<size_t, UID> get_subscription_key(const SubscriptionVariant &subscription);

    /**
     * @brief Find index of an entity type in its variant.
     * @details For example, you can use the method to find an index of a message type in a message variant or an index
     * of a subscription type in a subscription variant.
     * @tparam Variant variant of one or more entity types.
     * @tparam Type entity type to search.
     */
    template <typename Variant, typename Type>
    static constexpr size_t get_type_index = boost::mp11::mp_find<Variant, Type>::value;

public:
    MessageEndpoint(MessageEndpoint &&endpoint);
    MessageEndpoint &operator=(MessageEndpoint &&endpoint) = default;
    MessageEndpoint &operator=(MessageEndpoint &) = delete;
    virtual ~MessageEndpoint();

public:
    /**
     * @brief Add a subscription to messages of the specified type from senders with given UIDs.
     * @note If the subscription for the specified receiver and message type already exists, the method updates the list
     * of senders in the subscription.
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
     * @return true if a message was received, false if no message was received.
     */
    bool receive_message();

    /**
     * @brief Read messages of the specified type received via subscription.
     * @note After reading the messages, the method clears them from the subscription.
     * @tparam MessageType type of messages to read.
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
     * @brief Type of subscription container.
     */
    using SubscriptionContainer = std::map<std::pair<size_t, UID>, SubscriptionVariant>;

protected:
    class MessageEndpointImpl;
    /**
     * @brief Message endpoint implementation.
     */
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
