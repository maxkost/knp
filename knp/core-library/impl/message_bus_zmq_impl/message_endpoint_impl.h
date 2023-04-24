/**
 * @file message_endpoint_impl.h
 * @brief Message endpoint ZeroMQ implementation header.
 * @author Artiom N.
 * @date 31.03.2023
 */

#pragma once

#include <knp/core/message_endpoint.h>

#include <memory>
#include <string>
#include <vector>

#include <zmq.hpp>


namespace knp::core
{

class MessageEndpoint::MessageEndpointImpl
{
public:
    explicit MessageEndpointImpl(zmq::socket_t &&sub_socket, zmq::socket_t &&pub_socket);

public:
    void send_message(const std::vector<uint8_t> &data);
    void send_message(const void *data, size_t size);
    std::optional<zmq::message_t> receive_message();

private:
    // zmq::context_t &context_;
    zmq::socket_t sub_socket_;
    zmq::socket_t pub_socket_;
};

}  // namespace knp::core
