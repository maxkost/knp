/**
 * @file message_bus_cpu_impl.h
 * @brief CPU-based message bus implementation header.
 * @author Vartenkov A.
 * @date 18.09.2023
 */
#pragma once

#include <knp/core/message_bus.h>

#include <message_bus_impl.h>

#include <list>
#include <memory>
#include <mutex>
#include <vector>

/**
 * @brief Namespace for implementations of message bus.
 */
namespace knp::core::messaging::impl
{
class MessageEndpointCPU;

class MessageBusCPUImpl : public MessageBusImpl
{
public:
    void update() override;
    size_t step() override;
    [[nodiscard]] core::MessageEndpoint create_endpoint() override;

private:
    // cppcheck-suppress unusedStructMember
    std::vector<knp::core::messaging::MessageVariant> messages_to_route_;
    // cppcheck-suppress unusedStructMember
    std::list<std::weak_ptr<MessageEndpointCPU>> endpoints_;
    std::mutex mutex_;
};
}  // namespace knp::core::messaging::impl
