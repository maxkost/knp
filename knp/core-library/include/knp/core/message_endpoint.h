/**
 * @file message_bus.h
 * @brief Message bus interface.
 * @author Artiom N.
 * @date 23.01.2023
 */

#pragma once

#include <knp/core/messaging.h>
#include <knp/core/subscription.h>
#include <knp/core/uid.h>

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
    using SupportedMessages = messaging::AllMessages;
    using AllSubscriptions = boost::mp11::mp_transform<Subscription, SupportedMessages>;

    using SubscriptionVariant = boost::mp11::mp_rename<AllSubscriptions, std::variant>;
    using MessageVariant = boost::mp11::mp_rename<SupportedMessages, std::variant>;

public:
    static UID get_receiver_uid(const SubscriptionVariant &subscription);
    static std::pair<size_t, UID> get_subscription_key(const SubscriptionVariant &subscription);

    static UID get_receiver_uid1(MessageEndpoint::SubscriptionVariant subscription)
    {
        return std::visit([](auto &v) { return std::decay_t<decltype(v)>(v).get_receiver_uid(); }, subscription);
    }

    template <typename Variant, typename Type>
    static constexpr size_t get_type_index = boost::mp11::mp_find<Variant, Type>::value;

public:
    MessageEndpoint(MessageEndpoint &&endpoint);
    MessageEndpoint &operator=(MessageEndpoint &&endpoint) = default;
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
    void send_message(const MessageVariant &message);

    /**
     * @brief Receive a message from the message bus.
     * @return true if nonempty message was received, false otherwise.
     */
    bool receive_message();

    /**
     * @brief Get a subscription to a message type by its receiver uid
     * @param receiver_uid UID of the receiving object
     */
    template <class MessageType>
    Subscription<MessageType> &get_subscription(const knp::core::UID &receiver_uid)
    {
        constexpr size_t index = get_type_index<MessageVariant, MessageType>;
        return subscriptions_[std::make_pair(index, receiver_uid)];
    }


    /**
     * @brief Receive messages in the cycle.
     */
    void receive_all_messages();

public:
    using SubscriptionContainer = std::map<std::pair<size_t, UID>, SubscriptionVariant>;

protected:
    explicit MessageEndpoint(void *context, const std::string &sub_addr, const std::string &pub_addr);

private:
    MessageEndpoint() = delete;

private:
    /// Message endpoint implementation.
    class MessageEndpointImpl;

    SubscriptionContainer subscriptions_;
    std::unique_ptr<MessageEndpointImpl> impl_;
};

}  // namespace knp::core
