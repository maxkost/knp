/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_bus.h>
#include <knp/core/messaging.h>

#include <spdlog/spdlog.h>

#include <zmq.hpp>

#include "message_bus_zmq_impl/message_bus_impl.h"


namespace
{

class MessageEndpointConstructible : public knp::core::MessageEndpoint
{
public:
    explicit MessageEndpointConstructible(
        zmq::context_t *context, const std::string &sub_addr, const std::string &pub_addr)
        : MessageEndpoint(context, sub_addr, pub_addr)
    {
    }
};

}  // namespace


namespace knp::core
{

MessageBus::MessageBus() : impl_(std::make_unique<MessageBusImpl>())
{
    assert(impl_.get());
}


MessageBus::~MessageBus() {}


MessageEndpoint MessageBus::get_endpoint()
{
    return impl_->get_endpoint();
}


bool MessageBus::step()
{
    return impl_->step();
}


size_t MessageBus::route_messages()
{
    size_t count = 0;
    while (step()) ++count;
    return count;
}

}  // namespace knp::core
