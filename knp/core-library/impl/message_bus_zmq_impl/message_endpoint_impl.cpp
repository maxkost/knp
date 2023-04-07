/**
 * @file message_endpoint_impl.cpp
 * @brief Message endpoint ZeroMQ implementation.
 * @author Artiom N.
 * @date 31.03.2023
 */

#include "message_endpoint_impl.h"

#include <spdlog/spdlog.h>

#include <memory>

#include <zmq.hpp>


namespace knp::core
{

MessageEndpoint::MessageEndpointImpl::MessageEndpointImpl(
    zmq::context_t &context, const std::string &sub_addr, const std::string &pub_addr)
    :  // context_(context),
      sub_socket_(context, zmq::socket_type::sub),
      pub_socket_(context, zmq::socket_type::dealer),
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
    pub_socket_.setsockopt(ZMQ_PROBE_ROUTER, 0);
#pragma GCC diagnostic pop
    // #endif
    sub_socket_.connect(sub_addr);
}


void MessageEndpoint::MessageEndpointImpl::send_message(const std::vector<uint8_t> &data)
{
    send_message(data.data(), data.size());
}


void MessageEndpoint::MessageEndpointImpl::send_message(const void *data, size_t size)
{
    // send_result is an optional and if it doesn't contain a value, EAGAIN was returned by the call.
    zmq::send_result_t result;
    try
    {
        SPDLOG_DEBUG("Endpoint sending message");
        do
        {
            SPDLOG_INFO("Sending {} bytes", size);
            result = pub_socket_.send(zmq::message_t(data, size), zmq::send_flags::none);
            SPDLOG_INFO("{} bytes was sent", size);
        } while (!result.has_value());
    }
    catch (const zmq::error_t &e)
    {
        SPDLOG_CRITICAL(e.what());
        throw;
    }
}


std::optional<zmq::message_t> MessageEndpoint::MessageEndpointImpl::receive_message()
{
    zmq::message_t msg;
    // recv_result is an optional and if it doesn't contain a value, EAGAIN was returned by the call.
    zmq::recv_result_t result;
    using std::chrono_literals::operator""ms;

    try
    {
        SPDLOG_DEBUG("Endpoint receiving message");

        std::array<zmq_pollitem_t, 1> items = {zmq_pollitem_t{.socket = sub_socket_.handle(), .events = ZMQ_POLLIN}};

        if (zmq::poll<1>(items, 1ms))
        {
            do
            {
                result = sub_socket_.recv(msg, zmq::recv_flags::dontwait);
            } while (!result.has_value());
        }
    }
    catch (const zmq::error_t &e)
    {
        SPDLOG_CRITICAL(e.what());
        throw;
    }

    // Always has value.
    if (!result.has_value() || !result.value()) return std::nullopt;

    return msg;
}
}  // namespace knp::core
