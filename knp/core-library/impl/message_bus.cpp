/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_bus.h>
#include <knp/core/messaging.h>


namespace knp::core
{

template <typename MessageType>
UID subscribe(const UID &publisher_uid, std::function<void(const MessageType &)> callback)
{
}


void MessageBus::unsubscribe(const UID &subscription_uid) {}


template <typename MessageType>
void send_message(const MessageType &message)
{
}


template <typename MessageType>
void send_message(MessageType &&message)
{
}


}  // namespace knp::core
