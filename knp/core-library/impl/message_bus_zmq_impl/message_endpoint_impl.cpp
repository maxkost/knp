/**
 * @file message_endpoint_impl.cpp
 * @brief Message endpoint ZeroMQ implementation.
 * @author Artiom N.
 * @date 31.03.2023
 */

#include "message_endpoint_impl.h"

#include <memory>

#include <zmq.hpp>


namespace knp::core
{

MessageEndpoint::MessageEndpointImpl::MessageEndpointImpl(
    zmq::context_t &context, const std::string &sub_addr, const std::string &pub_addr)
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


void MessageEndpoint::MessageEndpointImpl::publish(const std::vector<uint8_t> &data)
{
    pub_socket_.send(zmq::buffer(data), zmq::send_flags::dontwait);
}
void MessageEndpoint::MessageEndpointImpl::send_message(const void *data, size_t size)
{
    pub_socket_.send(zmq::message_t(data, size), zmq::send_flags::dontwait);
}


std::optional<MessageEndpoint::MessageVariant> MessageEndpoint::MessageEndpointImpl::receive_message()
{
    zmq::message_t msg;
    auto recv_result = sub_socket_.recv(msg, zmq::recv_flags::dontwait);
    if (recv_result.has_value()) return std::optional<MessageVariant>{};
    return *msg.data<MessageVariant>();
}
}  // namespace knp::core
