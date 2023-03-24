/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_endpoint.h>

#include <memory>

#include <zmq.hpp>


namespace knp::core
{

UID get_receiver(const SubscriptionVariant &subscription)
{
    switch (subscription.index())
    {
        case 0:
            return std::get<0>(subscription).get_receiver();
        case 1:
            return std::get<1>(subscription).get_receiver();
        default:
            throw std::runtime_error("Unknown subscription type: " + std::to_string(subscription.index()));
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
          sub_socket_(context, zmq::socket_type::sub),
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


template <typename MessageType>
void MessageEndpoint::send_message(const MessageType &message)
{
    impl_->send_message(&message, sizeof(message));
}


template <typename MessageType>
MessageType MessageEndpoint::receive_message()
{
}


template <>
void MessageEndpoint::send_message<messaging::SpikeMessage>(const messaging::SpikeMessage &message);
template <>
void MessageEndpoint::send_message<messaging::SynapticImpactMessage>(const messaging::SynapticImpactMessage &message);


}  // namespace knp::core
