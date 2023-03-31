/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_bus.h>
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

class MessageBus::MessageBusImpl
{
public:
    MessageBusImpl()
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
    bool step()
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

    [[nodiscard]] MessageEndpoint get_endpoint()
    {
        return MessageEndpointConstructible(&context_, publish_sock_address_, router_sock_address_);
    }

private:
    std::string router_sock_address_;
    std::string publish_sock_address_;
    zmq::context_t context_;
    zmq::socket_t router_socket_;
    zmq::socket_t publish_socket_;
};


MessageBus::MessageBus() : impl_(std::make_unique<MessageBusImpl>())
{
    assert(impl_.get());
}


MessageBus::~MessageBus() {}


MessageEndpoint MessageBus::get_endpoint()
{
    return impl_->get_endpoint();
}

}  // namespace knp::core
