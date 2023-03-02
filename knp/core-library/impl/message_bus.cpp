/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_bus.h>
#include <knp/core/messaging.h>

#include <zmq.hpp>


namespace
{

class MessageEndpointConstructible : public knp::core::MessageEndpoint
{
public:
    explicit MessageEndpointConstructible(zmq::context_t *context) : MessageEndpoint(context) {}
};

}  // namespace


namespace knp::core
{

class MessageBus::MessageBusImpl
{
public:
    MessageBusImpl() : listening_socket_(context_, zmq::socket_type::push)
    {
        std::string sock_address{"inproc://"};

        // TODO: replace with std::format.
        sock_address += std::to_string(UID());

        listening_socket_.bind(sock_address);
    }

    MessageEndpoint &&get_endpoint() { return std::move(MessageEndpointConstructible(&context_)); }

private:
    zmq::context_t context_;
    zmq::socket_t listening_socket_;
};


MessageEndpoint &&MessageBus::get_endpoint()
{
    return std::move(impl_->get_endpoint());
}

}  // namespace knp::core
