/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_endpoint.h>

#include <memory>

#include <zmq.hpp>

#define RETURN_BY_INDEX_CASE(name, index) \
    case (index):                         \
        return std::get<index>(name)

namespace knp::core
{
namespace msg = messaging;

UID get_receiver(const SubscriptionVariant &subscription)
{
    // For
    switch (subscription.index())
    {
        RETURN_BY_INDEX_CASE(subscription, 0).get_receiver();
        RETURN_BY_INDEX_CASE(subscription, 1).get_receiver();
        default:
            throw std::runtime_error(
                "Unknown subscription index: " + std::to_string(subscription.index()) +
                ". Expand subscription index lists.");
    }
}


msg::MessageHeader get_header(const msg::MessageVariant &message)
{
    switch (message.index())
    {
        RETURN_BY_INDEX_CASE(message, 0).header_;
        RETURN_BY_INDEX_CASE(message, 1).header_;
        default:
            throw std::runtime_error(
                "Unknown message index: " + std::to_string(message.index()) + ". Expand message index lists.");
    }
}


std::pair<UID, size_t> get_subscription_key(const SubscriptionVariant &subscription)
{
    return std::make_pair(get_receiver(subscription), subscription.index());
}


class MessageEndpoint::MessageEndpointImpl
{
public:
    explicit MessageEndpointImpl(zmq::context_t &context, const std::string &sub_addr, const std::string &pub_addr)
        :  // context_(context),
          sub_socket_(context, zmq::socket_type::xsub),
          pub_socket_(context, zmq::socket_type::dealer),
          sub_addr_(sub_addr),
          pub_addr_(pub_addr)
    {
        pub_socket_.connect(pub_addr);

// #if (ZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 3, 4))
//         sub_socket_.set(zmq::sockopt::subscribe, "");
// #else
//  Strange version inconsistence: set() exists on Manjaro, but doesn't exist on Debian in the same library versions.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        sub_socket_.setsockopt(ZMQ_SUBSCRIBE, "");
#pragma GCC diagnostic pop
        // #endif
        sub_socket_.connect(sub_addr);
    }

public:
    void publish(const std::vector<uint8_t> &data) { pub_socket_.send(zmq::buffer(data), zmq::send_flags::dontwait); }
    void send_message(const void *data, size_t size)
    {
        pub_socket_.send(zmq::message_t(data, size), zmq::send_flags::dontwait);
    }

    std::optional<msg::MessageVariant> receive_message()
    {
        zmq::message_t msg;
        auto recv_result = sub_socket_.recv(msg, zmq::recv_flags::dontwait);
        if (recv_result.has_value()) return std::optional<msg::MessageVariant>{};
        return *msg.data<msg::MessageVariant>();
    }

private:
    // zmq::context_t &context_;
    zmq::socket_t sub_socket_;
    zmq::socket_t pub_socket_;
    std::string sub_addr_;
    std::string pub_addr_;
};


MessageEndpoint::MessageEndpoint(MessageEndpoint &&endpoint) : impl_(std::move(endpoint.impl_)) {}


MessageEndpoint::MessageEndpoint(void *context, const std::string &sub_addr, const std::string &pub_addr)
    : impl_(std::make_unique<MessageEndpointImpl>(*static_cast<zmq::context_t *>(context), sub_addr, pub_addr))
{
}


MessageEndpoint::~MessageEndpoint() {}


void MessageEndpoint::remove_receiver(const UID &receiver_uid)
{
    for (auto sub_iter = subscriptions_.begin(); sub_iter != subscriptions_.end(); ++sub_iter)
    {
        if (receiver_uid == get_receiver(*sub_iter)) subscriptions_.erase(sub_iter);
    }
}


void MessageEndpoint::send_message(const msg::MessageVariant &message)
{
    impl_->send_message(&message, sizeof(msg::MessageVariant));
}

template <>
bool MessageEndpoint::receive_message<msg::SpikeMessage>()
{
    std::optional<messaging::MessageVariant> message_var = impl_->receive_message();
    if (!message_var) return false;

    constexpr size_t index = get_type_index<SubscriptionVariant, msg::SpikeMessage>;
    auto const iter_pair = subscriptions_.get<by_type>().equal_range(index);
    auto &message = std::get<msg::SpikeMessage>(message_var.value());
    UID sender_uid = message.header_.sender_uid_;

    // Find a subscription you need
    for (auto iter = iter_pair.first; iter != iter_pair.second; ++iter)
    {
        const SubscriptionVariant &sub_variant = *iter;
        auto subscription = std::get<Subscription<msg::SpikeMessage>>(sub_variant);
        if (subscription.has_sender(sender_uid))
        {
            subscription.add_message(message);
        }
    }
}


}  // namespace knp::core
