/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_bus.h>

#include <spdlog/spdlog.h>

#include <zmq.hpp>

#include "message_bus_zmq_impl/message_bus_impl.h"


namespace knp::core
{

MessageBusImpl::~MessageBusImpl() {}

void MessageBusImpl::update() {}


MessageBus::MessageBus() : impl_(std::make_unique<MessageBusZMQImpl>())
{
    assert(impl_.get());
}


MessageBus::MessageBus(std::unique_ptr<MessageBusImpl> &&impl) : impl_(std::move(impl))
{
    if (!impl_) throw std::runtime_error("Unavailable message bus implementation");
}


MessageEndpoint MessageBus::create_endpoint()
{
    return impl_->create_endpoint();
}


bool MessageBus::step()
{
    return impl_->step();
}


size_t MessageBus::route_messages()
{
    SPDLOG_DEBUG("Message routing cycle started...");
    size_t count = 0;
    impl_->update();
    while (step())
    {
        ++count;
    }
    return count;
}

}  // namespace knp::core
