/**
 * @file message_bus_impl.cpp
 * @brief Message bus implementation.
 * @author Vartenkov A.
 * @date 19.09.2023
 */

#include <knp/core/message_bus_impl.h>

namespace knp::core::messaging::impl
{
MessageBusImpl::~MessageBusImpl() {}

size_t MessageBusImpl::step()
{
    return 0;
}

void MessageBusImpl::update() {}

}  // namespace knp::core::messaging::impl
