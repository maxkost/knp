/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_endpoint.h>
#include <knp/core/messaging.h>


namespace knp::core
{


class MessageEndpoint::MessageEndpointImpl
{
public:
    MessageEndpointImpl()
    { /*sock.send(zmq::str_buffer("Hello, world"), zmq::send_flags::dontwait);*/
    }

private:
    zmq::context_t context_;
};


template <typename MessageType>
UID MessageEndpoint::subscribe(const UID &publisher_uid, std::function<void(const MessageType &)> callback)
{
}


void MessageEndpoint::unsubscribe(const UID &subscription_uid) {}


template <>
void MessageEndpoint::send_message<>(const MessageType &message)
{
}


template <>
void MessageEndpoint::send_message<>(MessageType &&message)
{
}


}  // namespace knp::core
