/**
 * @file message_bus_single_cpu_impl.h
 * @brief Single-threaded CPU message bus implementation header.
 * @author Vartenkov A.
 * @date 18.09.2023
 */
#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <vector>

#include "knp/core/message_bus.h"

/**
 * @brief Namespace for implementations of message bus.
 */
namespace knp::core::messaging::impl
{
class MessageEndpointCPUImpl;

class MessageBusCPUImpl : public MessageBusImpl
{
public:
    void update() override;
    size_t step() override;
    [[nodiscard]] core::MessageEndpoint create_endpoint() override;

private:
    std::vector<knp::core::messaging::MessageVariant> messages_to_route_;
    std::list<std::weak_ptr<MessageEndpointCPUImpl>> endpoints_;
    std::mutex mutex_;
};
}  // namespace knp::core::messaging::impl
