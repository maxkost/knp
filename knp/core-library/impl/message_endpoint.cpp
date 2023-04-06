/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_endpoint.h>

#include <spdlog/spdlog.h>

#include <memory>

#include <boost/preprocessor.hpp>

#include "message_bus_zmq_impl/message_endpoint_impl.h"


namespace knp::core
{

UID MessageEndpoint::get_receiver_uid(const MessageEndpoint::SubscriptionVariant &subscription)
{
    return std::visit([](auto &v) { return std::decay_t<decltype(v)>(v).get_receiver_uid(); }, subscription);
}


messaging::MessageHeader get_header(const MessageEndpoint::MessageVariant &message)
{
    return std::visit([](auto &v) { return std::decay_t<decltype(v)>(v).header_; }, message);
}


bool operator<(const MessageEndpoint::SubscriptionVariant &sv1, const MessageEndpoint::SubscriptionVariant &sv2)
{
    return MessageEndpoint::get_receiver_uid(sv1) < MessageEndpoint::get_receiver_uid(sv2);
}


std::pair<UID, size_t> MessageEndpoint::get_subscription_key(const MessageEndpoint::SubscriptionVariant &subscription)
{
    return std::make_pair(get_receiver_uid(subscription), subscription.index());
}


MessageEndpoint::MessageEndpoint(MessageEndpoint &&endpoint) : impl_(std::move(endpoint.impl_)) {}


MessageEndpoint::MessageEndpoint(void *context, const std::string &sub_addr, const std::string &pub_addr)
    : impl_(std::make_unique<MessageEndpointImpl>(*static_cast<zmq::context_t *>(context), sub_addr, pub_addr))
{
}


MessageEndpoint::~MessageEndpoint() {}


template <typename MessageType>
Subscription<MessageType> MessageEndpoint::subscribe(const UID &receiver, const std::vector<UID> &senders)
{
    SPDLOG_DEBUG("Subscribing {} to the list of senders...", std::string(receiver));

    //    constexpr auto index = get_type_index<MessageVariant, MessageType>;

    //    auto iter = subscriptions_.find(std::make_tuple(receiver, index));
    //    (void)iter;
    //    if (iter != subscriptions_.get<by_type_and_uid>().end())
    //    {
    //        auto &sub = *const_cast<Subscription<MessageType>*>(&std::get<index>(*iter));
    //        sub.add_senders(senders);
    //        return sub;
    //    }
    //    else
    {
        //        auto p = SubscriptionVariant(Subscription<MessageType>{receiver, senders});
        //        auto e_res = subscriptions_.insert(p);
        //        auto &sub = *const_cast<Subscription<MessageType>*>(&std::get<index>(*e_res.first));
    }

    std::vector<UID> v;
    UID ruid;
    return Subscription<MessageType>(ruid, v);
}


template <typename MessageType>
void MessageEndpoint::unsubscribe(const UID &receiver)
{
    SPDLOG_DEBUG("Unsubscribing {}...", std::string(receiver));

    //    constexpr auto index = get_type_index<MessageVariant, MessageType>;

    //    auto &sub_list = subscriptions_.get<by_type_and_uid>();
    //    sub_list.erase(sub_list.find(std::make_pair(receiver, index)));
}


void MessageEndpoint::remove_receiver(const UID &receiver)
{
    SPDLOG_DEBUG("Removing receiver {}...", std::string(receiver));

    for (auto sub_iter = subscriptions_.begin(); sub_iter != subscriptions_.end(); ++sub_iter)
    {
        if (get_receiver_uid(*sub_iter) == receiver) subscriptions_.erase(sub_iter);
    }
}


void MessageEndpoint::send_message(const MessageEndpoint::MessageVariant &message)
{
    SPDLOG_TRACE("Sending message from the {}...", std::string(get_header(message).sender_uid_));
    impl_->send_message(&message, sizeof(MessageVariant));
}


bool MessageEndpoint::receive_message()
{
    SPDLOG_TRACE("Receiving message...");

    std::optional<MessageVariant> message_var = impl_->receive_message();
    if (!message_var.has_value()) return false;

    const UID &sender_uid = get_header(message_var.value()).sender_uid_;

    auto const [iter_first, iter_second] = subscriptions_.get<by_type>().equal_range(message_var->index());

    // Find a subscription.
    for (auto iter = iter_first; iter != iter_second; ++iter)
    {
        std::visit(
            [&sender_uid, &message_var](auto &subscription)
            {
                if (subscription.has_sender(sender_uid))
                {
                    using PT = std::decay_t<decltype(subscription)>;
                    static_cast<PT>(subscription).add_message(std::get<typename PT::MessageType>(*message_var));
                }
            },
            *iter);
    }

    return true;
}


void MessageEndpoint::receive_all_messages()
{
    while (receive_message())
    {
    }
}


#define INSTANCE_MESSAGES_FUNCTIONS(n, template_for_instance, message_type)       \
    ;                                                                             \
    template Subscription<message_type> MessageEndpoint::subscribe<message_type>( \
        const UID &receiver, const std::vector<UID> &senders);                    \
    template void MessageEndpoint::unsubscribe<message_type>(const UID &receiver);

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_MESSAGES_FUNCTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_MESSAGES));

}  // namespace knp::core
