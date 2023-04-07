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


bool MessageBus::MessageBusImpl::step()
{
    zmq::message_t message, id_message;
    zmq::recv_result_t recv_result;
    using std::chrono_literals::operator""ms;

    try
    {
        SPDLOG_INFO("Bus receiving message");
        // recv_result is an optional and if it doesn't contain a value, EAGAIN was returned by the call.
        std::array<zmq_pollitem_t, 1> items = {zmq_pollitem_t{.socket = router_socket_.handle(), .events = ZMQ_POLLIN}};

        SPDLOG_INFO("Running poll()");
        if (zmq::poll<1>(items, 1ms))
        {
            SPDLOG_INFO("Poll() successfull, receiving data");
            do
            {
                recv_result = router_socket_.recv(message, zmq::recv_flags::dontwait);

                if (recv_result.has_value())
                    SPDLOG_INFO("Bus recieved {} bytes", recv_result.value());
                else
                    SPDLOG_INFO("Bus receiving error [EAGAIN]!");
            } while (!recv_result.has_value());
        }
        else
            return false;

        // TODO: Remove this.
        if (recv_result.value() == 5)
        {
            SPDLOG_INFO("ID was received...");
            return true;
        }

        SPDLOG_INFO("Data was received, bus re-sending message");
        // send_result is an optional and if it doesn't contain a value, EAGAIN was returned by the call.
        zmq::send_result_t send_result;
        do
        {
            send_result = publish_socket_.send(message, zmq::send_flags::dontwait);
        } while (!send_result.has_value());
        SPDLOG_DEBUG("Message was sent {}...", send_result.value());
    }
    catch (const zmq::error_t &e)
    {
        SPDLOG_CRITICAL(e.what());
        throw;
    }

    return recv_result.has_value() && recv_result.value() != 0;
}


[[nodiscard]] MessageEndpoint MessageBus::MessageBusImpl::get_endpoint()
{
    return MessageEndpointConstructible(&context_, publish_sock_address_, router_sock_address_);
}

}  // namespace knp::core
