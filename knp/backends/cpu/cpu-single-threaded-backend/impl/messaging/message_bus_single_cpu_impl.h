/**
 * @file message_bus_single_cpu_impl.h
 * @brief Single-threaded CPU message bus implementation header.
 * @author Vartenkov A.
 * @date 18.09.2023
 */
#pragma once

#include <knp/core/message_bus.h>

/**
 * @brief Namespace for single-threaded backend.
 */
namespace knp::backends::single_threaded_cpu
{
class MessageBusSingleCPUImpl : public core::MessageBusImpl
{
public:
    bool step() override;
    [[nodiscard]] core::MessageEndpoint create_endpoint() override;

private:
};
}  // namespace knp::backends::single_threaded_cpu
