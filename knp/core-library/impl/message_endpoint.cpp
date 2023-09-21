/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_endpoint.h>

#include <spdlog/spdlog.h>

#include <memory>

// sleep_for.
#include <thread>

#include <boost/preprocessor.hpp>


namespace knp::core
{

UID MessageEndpoint::get_receiver_uid(const MessageEndpoint::SubscriptionVariant &subscription)
{
    return std::visit([](const auto &subscr) { return subscr.get_receiver_uid(); }, subscription);
}


messaging::MessageHeader get_header(const knp::core::messaging::MessageVariant &message)
{
    return std::visit([](const auto &msg) { return msg.header_; }, message);
}


bool operator<(const MessageEndpoint::SubscriptionVariant &sv1, const MessageEndpoint::SubscriptionVariant &sv2)
{
    return MessageEndpoint::get_receiver_uid(sv1) < MessageEndpoint::get_receiver_uid(sv2);
}


std::pair<size_t, UID> MessageEndpoint::get_subscription_key(const MessageEndpoint::SubscriptionVariant &subscription)
{
    return std::make_pair(subscription.index(), get_receiver_uid(subscription));
}


MessageEndpoint::MessageEndpoint(MessageEndpoint &&endpoint)
    : impl_(std::move(endpoint.impl_)), subscriptions_(std::move(endpoint.subscriptions_))
{
}


MessageEndpoint::~MessageEndpoint() {}


template <typename MessageType>
Subscription<MessageType> &MessageEndpoint::subscribe(const UID &receiver, const std::vector<UID> &senders)
{
    SPDLOG_DEBUG("Subscribing {} to the list of senders...", std::string(receiver));

    constexpr size_t index = get_type_index<knp::core::messaging::MessageVariant, MessageType>;

    auto iter = subscriptions_.find(std::make_pair(index, receiver));
    if (iter != subscriptions_.end())
    {
        auto &sub = *const_cast<Subscription<MessageType> *>(&std::get<index>(iter->second));
        sub.add_senders(senders);
        return sub;
    }

    auto sub_variant = SubscriptionVariant{Subscription<MessageType>{receiver, senders}};
    auto insert_res = subscriptions_.insert(std::make_pair(std::make_pair(index, receiver), sub_variant));
    auto &sub = *const_cast<Subscription<MessageType> *>(&std::get<index>(insert_res.first->second));
    return sub;
}


template <typename MessageType>
bool MessageEndpoint::unsubscribe(const UID &receiver)
{
    SPDLOG_DEBUG("Unsubscribing {}...", std::string(receiver));
    constexpr auto index = get_type_index<knp::core::messaging::MessageVariant, MessageType>;
    auto iter = subscriptions_.find(std::make_pair(index, receiver));
    if (iter != subscriptions_.end())
    {
        subscriptions_.erase(iter);
        return true;
    }
    return false;
}


void MessageEndpoint::remove_receiver(const UID &receiver)
{
    SPDLOG_DEBUG("Removing receiver {}...", std::string(receiver));

    for (auto sub_iter = subscriptions_.begin(); sub_iter != subscriptions_.end(); ++sub_iter)
    {
        if (get_receiver_uid(sub_iter->second) == receiver) subscriptions_.erase(sub_iter);
    }
}


void MessageEndpoint::send_message(const knp::core::messaging::MessageVariant &message)
{
    SPDLOG_TRACE(
        "Sending message from the {}, index = {}...", std::string(get_header(message).sender_uid_), message.index());
    impl_->send_message(message);
}


bool MessageEndpoint::receive_message()
{
    SPDLOG_DEBUG("Receiving message...");

    auto message_opt = impl_->receive_message();
    if (!message_opt.has_value()) return false;
    auto &message = message_opt.value();
    const UID &sender_uid = get_header(message).sender_uid_;
    const size_t type_index = message.index();

    // Find a subscription.
    for (auto &&[k, sub_variant] : subscriptions_)
    {
        if (sub_variant.index() != type_index)
        {
            SPDLOG_TRACE(
                "Subscription message type index != message type index [{} != {}]", sub_variant.index(), type_index);
            continue;
        }

        std::visit(
            [&sender_uid, &message](auto &&subscription)
            {
                SPDLOG_TRACE("Sender UID = {}...", std::string(sender_uid));
                if (subscription.has_sender(sender_uid))
                {
                    SPDLOG_TRACE("Subscription has sender with UID = {}", std::string(sender_uid));
                    subscription.add_message(
                        std::get<typename std::decay_t<decltype(subscription)>::MessageType>(message));
                    SPDLOG_TRACE("Message was added to the subscription {}", std::string(sender_uid));
                }
            },
            sub_variant);

        // sub_variant.swap(v);
    }

    return true;
}


void MessageEndpoint::receive_all_messages(const std::chrono::milliseconds &sleep_duration)
{
    while (receive_message())
    {
        if (sleep_duration.count()) std::this_thread::sleep_for(sleep_duration);
    }
}


#define INSTANCE_MESSAGES_FUNCTIONS(n, template_for_instance, message_type)        \
    template Subscription<message_type> &MessageEndpoint::subscribe<message_type>( \
        const UID &receiver, const std::vector<UID> &senders);                     \
    template bool MessageEndpoint::unsubscribe<message_type>(const UID &receiver);

BOOST_PP_SEQ_FOR_EACH(INSTANCE_MESSAGES_FUNCTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_MESSAGES))

}  // namespace knp::core
