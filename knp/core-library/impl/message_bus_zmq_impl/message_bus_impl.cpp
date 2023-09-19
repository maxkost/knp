/**
 * @file message_bus_impl.cpp
 * @brief Message bus ZeroMQ implementation.
 * @author Artiom N.
 * @date 31.03.2023
 */

#include "message_bus_impl.h"

#include <spdlog/spdlog.h>

#include <memory>
#include <utility>
#include <vector>

#include <zmq.hpp>

#include "message_endpoint_impl.h"


namespace knp::core
{

class MessageEndpointZMQ : public MessageEndpoint
{
public:
    explicit MessageEndpointZMQ(zmq::socket_t &&sub_socket, zmq::socket_t &&pub_socket)
    {
        impl_ = std::make_shared<knp::core::MessageEndpointZMQImpl>(std::move(sub_socket), std::move(pub_socket));
    }
};


MessageBusZMQImpl::MessageBusZMQImpl()
    :  // TODO: replace with std::format.
      router_sock_address_("inproc://route_" + std::string(UID())),
      publish_sock_address_("inproc://publish_" + std::string(UID())),
      router_socket_(context_, zmq::socket_type::router),
      publish_socket_(context_, zmq::socket_type::pub)
{
    SPDLOG_DEBUG("Router socket binding to {}", router_sock_address_);
    router_socket_.bind(router_sock_address_);
    SPDLOG_DEBUG("Publish socket binding to {}", publish_sock_address_);
    publish_socket_.bind(publish_sock_address_);
    // zmq::proxy(router_socket_, publish_socket_);
}


bool MessageBusZMQImpl::step()
{
    zmq::message_t message;
    zmq::recv_result_t recv_result;
    using std::chrono_literals::operator""ms;

    try
    {
        // recv_result is an optional and if it doesn't contain a value, EAGAIN was returned by the call.
        std::vector<zmq_pollitem_t> items = {zmq_pollitem_t{.socket = router_socket_.handle(), .events = ZMQ_POLLIN}};

        SPDLOG_DEBUG("Running poll()");

        if (zmq::poll(items, 1ms))
        {
            SPDLOG_TRACE("Poll() successful, receiving data");
            do
            {
                recv_result = router_socket_.recv(message, zmq::recv_flags::dontwait);

                if (recv_result.has_value())
                    SPDLOG_TRACE("Bus recieved {} bytes", recv_result.value());
                else
                    SPDLOG_WARN("Bus receiving error [EAGAIN]!");
            } while (!recv_result.has_value());
        }
        else
        {
            SPDLOG_DEBUG("Poll() returned 0, exiting");
            return false;
        }

        if (isit_id(recv_result)) return true;

        SPDLOG_DEBUG("Data was received, bus will re-send the message");
        // send_result is an optional and if it doesn't contain a value, EAGAIN was returned by the call.
        zmq::send_result_t send_result;
        do
        {
            send_result = publish_socket_.send(message, zmq::send_flags::none);
        } while (!send_result.has_value());
        SPDLOG_TRACE("Bus sent {} bytes...", send_result.value());
    }
    catch (const zmq::error_t &e)
    {
        SPDLOG_CRITICAL(e.what());
        throw;
    }
    return recv_result.has_value() && recv_result.value() != 0;
}


MessageEndpoint MessageBusZMQImpl::create_endpoint()
{
    zmq::socket_t sub_socket{context_, zmq::socket_type::sub};
    zmq::socket_t pub_socket{context_, zmq::socket_type::dealer};

// #if (ZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 3, 4))
//         sub_socket_.set(zmq::sockopt::subscribe, "");
// #else
//  Strange version inconsistence: set() exists on Manjaro, but doesn't exist on Debian in the same library versions.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    // pub_socket_.setsockopt(ZMQ_PROBE_ROUTER, 0);
    //    std::string id{UID()};
    //    pub_socket.setsockopt(ZMQ_IDENTITY, id.c_str(), 4);

    sub_socket.setsockopt(ZMQ_SUBSCRIBE, nullptr, 0);
#pragma GCC diagnostic pop
    // #endif
    SPDLOG_DEBUG("Pub socket connecting to {}", router_sock_address_);
    pub_socket.connect(router_sock_address_);
    SPDLOG_DEBUG("Sub socket connecting to {}", publish_sock_address_);
    sub_socket.connect(publish_sock_address_);

    return MessageEndpointZMQ(std::move(sub_socket), std::move(pub_socket));
}

}  // namespace knp::core
