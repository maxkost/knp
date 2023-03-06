/**
 * Message bus and MessageEndpoint testing.
 */

#include <knp/core/message_bus.h>

#include <tests_common.h>


TEST(MessageBusSuite, CreateBusAndEndpoint)
{
    knp::core::MessageBus bus;

    auto ep{bus.get_endpoint()};
}
