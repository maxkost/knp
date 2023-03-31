/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_endpoint.h>

#include <memory>

#include <zmq.hpp>

#include "message_bus_zmq_impl/message_endpoint_impl.h"


namespace knp::core
{

UID get_receiver(const MessageEndpoint::SubscriptionVariant &subscription)
{
    return std::visit([](auto &v) { return std::decay_t<decltype(v)>(v).get_receiver_uid(); }, subscription);
}


template <typename T, typename VT>
typename std::vector<VT>::iterator MessageEndpoint::find_elem(const knp::core::UID &uid, std::vector<VT> &container)
{
    auto result = std::find_if(
        container.begin(), container.end(),
        [&uid](VT &p_variant) -> bool
        {
            constexpr auto type_n = boost::mp11::mp_find<VT, T>();
            if (p_variant.index() != type_n) return false;
            return uid == (std::get<type_n>(p_variant)).get_uid();
        });
    return result;
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
    constexpr size_t index = get_type_index<SubscriptionVariant, Subscription<MessageType>>;

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
    constexpr size_t index = get_type_index<MessageVariant, MessageType>();
    auto &sub_list = subscriptions_.get<by_type_and_uid>();
    sub_list.erase(sub_list.find(std::make_pair(receiver, index)));
}


void MessageEndpoint::remove_receiver(const UID &receiver_uid)
{
    // TODO: find
    for (auto sub_iter = subscriptions_.begin(); sub_iter != subscriptions_.end(); ++sub_iter)
    {
        if (receiver_uid == get_receiver(*sub_iter)) subscriptions_.erase(sub_iter);
    }
}


void MessageEndpoint::send_message(const MessageEndpoint::MessageVariant &message)
{
    impl_->send_message(&message, sizeof(MessageVariant));
}


template <>
bool MessageEndpoint::receive_message<messaging::SpikeMessage>()
{
    std::optional<MessageVariant> message_var = impl_->receive_message();
    if (!message_var) return false;

    constexpr size_t index = get_type_index<SubscriptionVariant, messaging::SpikeMessage>;

    auto const iter_pair = subscriptions_.get<by_type>().equal_range(index);
    //    UID sender_uid = message.header_.sender_uid_;
    //    auto &message = std::get<messaging::SpikeMessage>(message_var.value());

    // Find a subscription you need
    for (auto iter = iter_pair.first; iter != iter_pair.second; ++iter)
    {
        //        const SubscriptionVariant &sub_variant = *iter;
        //        auto subscription = std::get<Subscription<messaging::SpikeMessage>>(sub_variant);
        //        if (subscription.has_sender(sender_uid))
        //        {
        //            // subscription.add_message(message);
        //        }
    }

    return true;
}

}  // namespace knp::core
