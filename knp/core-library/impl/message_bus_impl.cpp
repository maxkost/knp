/**
 * @file message_bus_impl.cpp
 * @brief Message bus implementation.
 * @author Vartenkov A.
 * @date 19.09.2023
 */

#include <knp/core/message_bus_impl.h>

namespace knp::core
{
MessageBusImpl::~MessageBusImpl() {}

bool MessageBusImpl::step()
{
    return false;
}

void MessageBusImpl::update() {}

}  // namespace knp::core
