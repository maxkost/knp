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

#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 3, 4))
        sub_socket_.set(zmq::sockopt::subscribe, "");
#else
        sub_socket_.setsockopt(ZMQ_SUBSCRIBE, "");
#endif
        sub_socket_.connect(sub_addr);
    }

public:
    void publish(const std::vector<uint8_t> &data) { pub_socket_.send(zmq::buffer(data), zmq::send_flags::dontwait); }
    void send_message(const void *data, size_t size)
    {
        pub_socket_.send(zmq::message_t(data, size), zmq::send_flags::dontwait);
    }

    void subscribe() {}

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
UID MessageEndpoint::subscribe(const UID &publisher_uid, std::function<void(const MessageType &)> callback)
{
    return UID();
}


void MessageEndpoint::unsubscribe(const UID &subscription_uid) {}


template <typename MessageType>
void MessageEndpoint::send_message(const MessageType &message)
{
    impl_->send_message(&message, sizeof(message));
}


template <typename MessageType>
MessageType MessageEndpoint::receive_message()
{
}


// Instantiation.
template <>
UID MessageEndpoint::subscribe<messaging::SpikeMessage>(
    const UID &publisher_uid, std::function<void(const messaging::SpikeMessage &)> callback);

template <>
void MessageEndpoint::send_message<messaging::SpikeMessage>(const messaging::SpikeMessage &message);
template <>
void MessageEndpoint::send_message<messaging::SynapticImpactMessage>(const messaging::SynapticImpactMessage &message);


}  // namespace knp::core
