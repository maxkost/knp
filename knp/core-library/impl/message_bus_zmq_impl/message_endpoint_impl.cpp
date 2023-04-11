/**
 * @file message_endpoint_impl.cpp
 * @brief Message endpoint ZeroMQ implementation.
 * @author Artiom N.
 * @date 31.03.2023
 */

#include "message_endpoint_impl.h"

#include <spdlog/spdlog.h>

#include <memory>
#include <utility>

#include <zmq.hpp>


namespace knp::core
{

MessageEndpoint::MessageEndpointImpl::MessageEndpointImpl(zmq::socket_t &&sub_socket, zmq::socket_t &&pub_socket)
    : sub_socket_(std::move(sub_socket)), pub_socket_(std::move(pub_socket))
{
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
            SPDLOG_TRACE("Sending {} bytes", size);
            result = pub_socket_.send(zmq::message_t(data, size), zmq::send_flags::dontwait);
            SPDLOG_TRACE("{} bytes was sent", size);
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

        SPDLOG_DEBUG("Running poll()");
        if (zmq::poll<1>(items, 1ms))
        {
            SPDLOG_TRACE("Poll() successful, receiving data");
            do
            {
                result = sub_socket_.recv(msg, zmq::recv_flags::dontwait);

                if (result.has_value())
                    SPDLOG_TRACE("Endpoint received {} bytes", result.value());
                else
                    SPDLOG_WARN("Endpoint receiving error [EAGAIN]!");
            } while (!result.has_value());
        }
        else
        {
            SPDLOG_DEBUG("Poll() returned 0, exiting");
            return std::nullopt;
        }
    }
    catch (const zmq::error_t &e)
    {
        SPDLOG_CRITICAL(e.what());
        throw;
    }

    return msg;
}
}  // namespace knp::core
