/**
 * Message bus and MessageEndpoint testing.
 */

#include <knp/core/message_bus.h>
#include <knp/core/messaging.h>

#include <tests_common.h>


TEST(MessageBusSuite, AddSubscriptionMessage)
{
    using SpikeMessage = knp::core::messaging::SpikeMessage;

    SpikeMessage msg{{knp::core::UID{}}, {1, 2, 3, 4, 5}};

    std::vector<knp::core::UID> senders = {msg.header_.sender_uid_};
    knp::core::Subscription<SpikeMessage> sub{knp::core::UID(), senders};

    sub.add_message(std::move(msg));

    const auto &msgs = sub.get_messages();

    EXPECT_EQ(msgs.size(), 1);
}


TEST(MessageBusSuite, CreateBusAndEndpoint)
{
    using SpikeMessage = knp::core::messaging::SpikeMessage;
    knp::core::MessageBus bus;

    auto ep1{bus.get_endpoint()};
    auto ep2{bus.get_endpoint()};

    // SpikeMessage msg{.header_{.sender_uid_{knp::core::UID()}}, .neuron_indexes_{1, 2, 3, 4, 5}};
    SpikeMessage msg{{knp::core::UID{}}, {1, 2, 3, 4, 5}};

    auto subscription = ep2.subscribe<SpikeMessage>(knp::core::UID(), {msg.header_.sender_uid_});

    ep1.send_message(msg);
    // ID message and data message.
    EXPECT_EQ(bus.route_messages(), 2);
    ep2.receive_all_messages();

    const auto &msgs = subscription.get_messages();

    EXPECT_EQ(msgs.size(), 1);
}
