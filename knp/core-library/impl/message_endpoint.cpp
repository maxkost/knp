/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_endpoint.h>

#include <spdlog/spdlog.h>

#include <memory>

#include "message_bus_zmq_impl/message_endpoint_impl.h"


namespace knp::core
{

UID get_receiver(const MessageEndpoint::SubscriptionVariant &subscription)
{
    return std::visit([](auto &v) { return std::decay_t<decltype(v)>(v).get_receiver_uid(); }, subscription);
}


messaging::MessageHeader get_header(const MessageEndpoint::MessageVariant &message)
{
    return std::visit([](auto &v) { return std::decay_t<decltype(v)>(v).header_; }, message);
}


std::pair<UID, size_t> get_subscription_key(const MessageEndpoint::SubscriptionVariant &subscription)
{
    return std::make_pair(get_receiver(subscription), subscription.index());
}


MessageEndpoint::MessageEndpoint(MessageEndpoint &&endpoint) : impl_(std::move(endpoint.impl_)) {}


MessageEndpoint::MessageEndpoint(void *context, const std::string &sub_addr, const std::string &pub_addr)
    : impl_(std::make_unique<MessageEndpointImpl>(*static_cast<zmq::context_t *>(context), sub_addr, pub_addr))
{
}


MessageEndpoint::~MessageEndpoint() {}


template <typename MessageType>
size_t MessageEndpoint::subscribe(const UID &receiver, const std::vector<UID> &senders)
{
    SPDLOG_DEBUG("Subscribing {} to the list of senders...", std::string(receiver));

    constexpr auto index = get_type_index<SubscriptionVariant, Subscription<MessageType>>();

    auto iter = subscriptions_.get<by_type_and_uid>().find(std::make_pair(receiver, index));
    if (iter != subscriptions_.get<by_type_and_uid>().end())
    {
        return std::get<index>(*iter).add_senders(senders);
    }
    else
    {
        subscriptions_.emplace(Subscription<MessageType>(receiver, senders));
        return senders.size();
    }
    return 0;
}


template <typename MessageType>
void MessageEndpoint::unsubscribe(const UID &receiver)
{
    SPDLOG_DEBUG("Unsubscribing {}...", std::string(receiver));

    constexpr auto index = get_type_index<MessageVariant, MessageType>();

    auto &sub_list = subscriptions_.get<by_type_and_uid>();
    sub_list.erase(sub_list.find(std::make_pair(receiver, index)));
}


void MessageEndpoint::remove_receiver(const UID &receiver)
{
    SPDLOG_DEBUG("Removing receiver {}...", std::string(receiver));

    for (auto sub_iter = subscriptions_.begin(); sub_iter != subscriptions_.end(); ++sub_iter)
    {
        if (get_receiver(*sub_iter) == receiver) subscriptions_.erase(sub_iter);
    }
}


void MessageEndpoint::send_message(const MessageEndpoint::MessageVariant &message)
{
    SPDLOG_TRACE("Sending message from the {}...", std::string(get_header(message).sender_uid_));
    impl_->send_message(&message, sizeof(MessageVariant));
}


template <typename MessageType>
bool MessageEndpoint::receive_message()
{
    SPDLOG_TRACE("Receiving message...");

    std::optional<MessageVariant> message_var = impl_->receive_message();
    if (!message_var) return false;

    constexpr size_t index = get_type_index<SubscriptionVariant, MessageType>;

    auto message = std::get<MessageType>(message_var.value());
    const UID &sender_uid = message.header_.sender_uid_;

    auto const [iter_first, iter_second] = subscriptions_.get<by_type>().equal_range(index);

    // Find a subscription you need
    for (auto iter = iter_first; iter != iter_second; ++iter)
    {
        auto subscription = std::get<Subscription<MessageType>>(*iter);

        if (subscription.has_sender(sender_uid))
        {
            subscription.add_message(message);
        }
    }

    return true;
}

}  // namespace knp::core
