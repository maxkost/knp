/**
 * Message bus and MessageEndpoint testing.
 */

#include <knp/core/message_bus.h>
#include <knp/core/messaging.h>

#include <tests_common.h>


TEST(MessageBusSuite, CreateBusAndEndpoint)
{
    using SpikeMessage = knp::core::messaging::SpikeMessage;
    knp::core::MessageBus bus;

    auto ep1{bus.get_endpoint()};
    auto ep2{bus.get_endpoint()};

    SpikeMessage msg{.header_{.sender_uid_{knp::core::UID()}}, .neuron_indexes_{1, 2, 3, 4, 5}};

    auto subscription = ep2.subscribe<SpikeMessage>(knp::core::UID(), {msg.header_.sender_uid_});

    EXPECT_NO_THROW(ep1.send_message(msg));

    EXPECT_EQ(bus.route_messages(), 1);
    ep2.receive_all_messages();

    const auto &msgs = subscription.get_messages();

    std::cout << msgs.size() << std::endl;
}
