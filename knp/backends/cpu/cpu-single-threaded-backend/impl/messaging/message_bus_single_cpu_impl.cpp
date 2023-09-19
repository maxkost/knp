/**
 * @file message_bus_single_cpu_impl.cpp
 * @brief Single-threaded CPU message bus implementation.
 * @author Vartenkov A.
 * @date 18.09.2023
 */

#include "message_bus_single_cpu_impl.h"

#include <utility>

#include "endpoint_single_cpu_impl.h"

/**
 * @brief Namespace for single-threaded backend.
 */
namespace knp::backends::single_threaded_cpu
{

class MessageEndpointSingleCPU : public knp::core::MessageEndpoint
{
public:
    explicit MessageEndpointSingleCPU(std::shared_ptr<MessageEndpointSingleCPUImpl> &&ptr) { impl_ = std::move(ptr); }
};

void MessageBusSingleCPUImpl::update()
{
    auto iter = endpoints_.begin();
    while (iter != endpoints_.end())
    {
        if (iter->expired())
        {
            auto new_iter = iter;
            ++new_iter;
            endpoints_.erase(iter);
            iter = new_iter;
            continue;
        }

        auto new_messages = iter->lock()->get_sent_messages();
        messages_to_route_.insert(
            messages_to_route_.end(), std::make_move_iterator(new_messages.begin()),
            std::make_move_iterator(new_messages.end()));
        ++iter;
    }
}


bool MessageBusSingleCPUImpl::step()
{
    if (messages_to_route_.empty()) return false;
    auto message = std::move(messages_to_route_.back());
    for (auto endpoint_ptr : endpoints_)
    {
        if (endpoint_ptr.expired()) continue;
        endpoint_ptr.lock()->add_received_message(message);
    }
    messages_to_route_.pop_back();
    return true;
}


core::MessageEndpoint MessageBusSingleCPUImpl::create_endpoint()
{
    auto endpoint_impl = std::make_shared<MessageEndpointSingleCPUImpl>();
    endpoints_.push_back(std::weak_ptr(endpoint_impl));
    return MessageEndpointSingleCPU(std::move(endpoint_impl));
}

}  // namespace knp::backends::single_threaded_cpu
