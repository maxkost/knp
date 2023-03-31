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


class MessageEndpoint::MessageEndpointImpl
{
public:
    explicit MessageEndpointImpl(zmq::context_t &context, const std::string &sub_addr, const std::string &pub_addr)
        :  // context_(context),
          sub_socket_(context, zmq::socket_type::sub),
          pub_socket_(context, zmq::socket_type::pub),
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

    std::optional<MessageVariant> receive_message()
    {
        zmq::message_t msg;
        auto recv_result = sub_socket_.recv(msg, zmq::recv_flags::dontwait);
        if (recv_result.has_value()) return std::optional<MessageVariant>{};
        return *msg.data<MessageVariant>();
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
