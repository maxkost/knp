/**
 * @file message_bus_impl.cpp
 * @brief Message bus ZeroMQ implementation.
 * @author Artiom N.
 * @date 31.03.2023
 */

#include "message_bus_impl.h"

#include <knp/core/messaging.h>

#include <spdlog/spdlog.h>

#include <zmq.hpp>


namespace
{

class MessageEndpointConstructible : public knp::core::MessageEndpoint
{
public:
    explicit MessageEndpointConstructible(
        zmq::context_t *context, const std::string &sub_addr, const std::string &pub_addr)
        : MessageEndpoint(context, sub_addr, pub_addr)
    {
    }
};

}  // namespace


namespace knp::core
{

MessageBus::MessageBusImpl::MessageBusImpl()
    :  // TODO: replace with std::format.
      router_sock_address_("inproc://route_" + std::string(UID())),
      publish_sock_address_("inproc://publish_" + std::string(UID())),
      router_socket_(context_, zmq::socket_type::router),
      publish_socket_(context_, zmq::socket_type::xpub)
{
    router_socket_.bind(router_sock_address_);
    publish_socket_.bind(publish_sock_address_);
    // zmq::proxy(router_socket_, publish_socket_);
}

/**
 * @brief send a single message from one socket to another
 */
bool MessageBus::MessageBusImpl::step()
{
    zmq::message_t message;
    if (router_socket_.recv(message))
    {
        const std::string e_msg = "Router socket can't receive message!";
        SPDLOG_CRITICAL(e_msg);
        throw std::logic_error(e_msg);
    }
    SPDLOG_DEBUG("Received message");  //  message.
    if (!publish_socket_.send(message, zmq::send_flags::dontwait))
    {
        const std::string e_msg = "Publication socket can't receive message!";
        SPDLOG_CRITICAL(e_msg);
        throw std::logic_error(e_msg);
    }
    return true;
}

[[nodiscard]] MessageEndpoint MessageBus::MessageBusImpl::get_endpoint()
{
    return MessageEndpointConstructible(&context_, publish_sock_address_, router_sock_address_);
}

}  // namespace knp::core
