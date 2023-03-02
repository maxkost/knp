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
    explicit MessageEndpointImpl(zmq::context_t &context)
        :  // context_(context),
          pub_socket_(context, zmq::socket_type::pub),
          sub_socket_(context, zmq::socket_type::sub)
    {
        /*sock.send(zmq::str_buffer("Hello, world"), zmq::send_flags::dontwait);*/
    }

public:
    void publish() {}

    void subscribe() {}

private:
    // zmq::context_t &context_;
    zmq::socket_t pub_socket_;
    zmq::socket_t sub_socket_;
};


MessageEndpoint::MessageEndpoint(void *context)
    : impl_(std::make_unique<MessageEndpointImpl>(*static_cast<zmq::context_t *>(context)))
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
}


template <typename MessageType>
void MessageEndpoint::send_message(MessageType &&message)
{
}


// Instantiation.
template <>
void MessageEndpoint::send_message<messaging::SpikeMessage>(messaging::SpikeMessage &&message);

template <>
void MessageEndpoint::send_message<messaging::SpikeMessage>(const messaging::SpikeMessage &message);

template <>
void MessageEndpoint::send_message<messaging::SynapticImpactMessage>(messaging::SynapticImpactMessage &&message);


}  // namespace knp::core
