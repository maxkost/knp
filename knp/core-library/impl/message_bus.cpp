/**
 * @file message_bus.cpp
 * @brief Message bus implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/core/message_bus.h>
#include <knp/core/messaging.h>

#include <zmq.hpp>


namespace knp::core
{


class MessageBus::MessageBusImpl
{
public:
    MessageBusImpl()
    {
        zmq::socket_t sock(ctx, zmq::socket_type::push);
        sock.bind("inproc://test");
    }

private:
    zmq::context_t context_;
};


MessageEndpoint &&MessageBus::get_endpoint() {}

}  // namespace knp::core
