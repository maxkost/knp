/**
 * @file message_bus.h
 * @brief Message bus interface.
 * @author Artiom N.
 * @date 23.01.2023
 */

#pragma once

#include <knp/core/message_endpoint.h>

#include <functional>
#include <memory>

/**
 * @brief Namespace for message bus implementations.
 */
namespace knp::core::messaging::impl
{
/**
 * @brief The MessageBusImpl class is an internal implementation class for message bus.
 */
class MessageBusImpl;
}  // namespace knp::core::messaging::impl


/**
 * @brief Core library namespace.
 */
namespace knp::core
{
/**
 * @brief The MessageBus class is a definition of an interface to a message bus.
 */
class MessageBus
{
public:
    /**
     * @brief Default message bus constructor.
     */
    MessageBus();

    /**
     * @brief Message bus constructor with selection of implementation.
     * @param is_impl_cpu determines if cpu implementation is used.
     */
    explicit MessageBus(bool is_impl_cpu);

    /**
     * @brief Message bus constructor with a specialized implementation.
     * @param impl message bus implementation.
     */
    explicit MessageBus(std::unique_ptr<messaging::impl::MessageBusImpl> &&impl);

    /**
     * @brief Message bus destructor.
     */
    ~MessageBus();

public:
    /**
     * @brief Create a new endpoint that sends and receives messages through the message bus.
     * @return new endpoint.
     * @see MessageEndpoint.
     */
    [[nodiscard]] MessageEndpoint create_endpoint();

    /**
     * @brief Route some messages.
     * @return number of messages routed during the step.
     */
    size_t step();

    /**
     * @brief Route messages.
     * @return number of messages routed.
     */
    size_t route_messages();

private:
    /**
     * @brief Create a CPU-based message bus implementation.
     * @return unique pointer to implementation.
     */
    static std::unique_ptr<messaging::impl::MessageBusImpl> make_cpu_implementation();

    /**
     * @brief Create a ZMQ-based message bus implementation.
     * @return unique pointer to implementation.
     */
    static std::unique_ptr<messaging::impl::MessageBusImpl> make_zmq_implementation();
    /**
     * @brief Message bus implementation.
     * @note currently two possible cases are ZMQ implementation and CPU implementation.
     */
    std::unique_ptr<messaging::impl::MessageBusImpl> impl_;
};

}  // namespace knp::core
