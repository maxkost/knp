/**
 * @file message_endpoint_zmq_impl.h
 * @brief Message endpoint ZeroMQ implementation header.
 * @author Artiom N.
 * @date 31.03.2023
 */

#pragma once

#include <knp/core/message_endpoint.h>

#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>

#include <zmq.hpp>


/**
 * @brief Namespace for implementations of message bus.
 */
namespace knp::core::messaging::impl
{

class MessageEndpointZMQImpl : public MessageEndpointImpl
{
public:
    explicit MessageEndpointZMQImpl(zmq::socket_t &&sub_socket, zmq::socket_t &&pub_socket);

public:
    std::optional<messaging::MessageVariant> receive_message() override
    {
        auto message_var = receive_zmq_message();
        if (!message_var.has_value()) return {};

        auto message = knp::core::messaging::extract_from_envelope(message_var->data());
        return message;
    }
    void send_message(const knp::core::messaging::MessageVariant &message) override
    {
        auto packed_msg = knp::core::messaging::pack_to_envelope(message);
        SPDLOG_TRACE("Packed message size = {}...", packed_msg.size());
        send_zmq_message(packed_msg.data(), packed_msg.size());
    }

public:
    void send_zmq_message(const std::vector<uint8_t> &data);
    void send_zmq_message(const void *data, size_t size);
    std::optional<zmq::message_t> receive_zmq_message();

private:
    // zmq::context_t &context_;
    zmq::socket_t sub_socket_;
    zmq::socket_t pub_socket_;
};

}  // namespace knp::core::messaging::impl
