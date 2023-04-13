/**
 * @file message_bus_impl.h
 * @brief Message bus ZeroMQ implementation header.
 * @author Artiom N.
 * @date 31.03.2023
 */

#pragma once

#include <knp/core/message_bus.h>

#include <spdlog/spdlog.h>

#include <string>

#include <zmq.hpp>


namespace knp::core
{

class MessageBus::MessageBusImpl
{
public:
    MessageBusImpl();

    /**
     * @brief send a single message from one socket to another
     */
    bool step();

    [[nodiscard]] MessageEndpoint create_endpoint();

private:
    std::string router_sock_address_;
    std::string publish_sock_address_;
    zmq::context_t context_;
    zmq::socket_t router_socket_;
    zmq::socket_t publish_socket_;
};


}  // namespace knp::core
