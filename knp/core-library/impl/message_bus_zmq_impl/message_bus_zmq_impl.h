/**
 * @file message_bus_zmq_impl.h
 * @brief Message bus ZeroMQ implementation header.
 * @author Artiom N.
 * @date 31.03.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <message_bus_impl.h>
#include <spdlog/spdlog.h>

#include <string>

#include <zmq.hpp>


namespace knp::core::messaging::impl
{

/**
 * @brief internal message bus class, not intended for user code.
 */
class MessageBusZMQImpl : public MessageBusImpl
{
public:
    MessageBusZMQImpl();

    /**
     * @brief send a message from one socket to another.
     */
    size_t step() override;

    /**
     * @brief Creates an endpoint that can be used for message exchange.
     * @return a new endpoint.
     */
    [[nodiscard]] MessageEndpoint create_endpoint() override;

private:
    bool isit_id(const zmq::recv_result_t &recv_result) const { return recv_result.value() == 5; }

private:
    /**
     * @brief Router socket address.
     */
    // cppcheck-suppress unusedStructMember
    std::string router_sock_address_;

    /**
     * @brief Publish socket address.
     */
    // cppcheck-suppress unusedStructMember
    std::string publish_sock_address_;

    /**
     * @brief Messaging context.
     */
    zmq::context_t context_;

    /**
     * @brief Router socket.
     */
    zmq::socket_t router_socket_;

    /**
     * @brief Publish socket.
     */
    zmq::socket_t publish_socket_;
};


}  // namespace knp::core::messaging::impl
