/**
 * Message bus and MessageEndpoint testing.
 */

#include <knp/core/message_bus.h>
#include <knp/core/messaging/messaging.h>

#include <tests_common.h>


TEST(MessageBusSuite, AddSubscriptionMessage)
{
    using SpikeMessage = knp::core::messaging::SpikeMessage;

    SpikeMessage msg{{knp::core::UID{}}, {1, 2, 3, 4, 5}};

    std::vector<knp::core::UID> senders = {msg.header_.sender_uid_};
    knp::core::Subscription<SpikeMessage> sub{knp::core::UID(), senders};

    EXPECT_EQ(sub.get_messages().size(), 0);

    sub.add_message(std::move(msg));

    const auto &msgs = sub.get_messages();

    EXPECT_EQ(msgs.size(), 1);
}


TEST(MessageBusSuite, CreateBusAndEndpointZMQ)
{
    using SpikeMessage = knp::core::messaging::SpikeMessage;
    knp::core::MessageBus bus(knp::core::MessageBus::make_zmq_implementation());

    auto ep1{bus.create_endpoint()};
    auto ep2{bus.create_endpoint()};

    SpikeMessage msg{{knp::core::UID{}}, {1, 2, 3, 4, 5}};

    auto &subscription = ep2.subscribe<SpikeMessage>(knp::core::UID(), {msg.header_.sender_uid_});

    ep1.send_message(msg);
    // ID message and data message.
    EXPECT_EQ(bus.route_messages(), 2);
    ep2.receive_all_messages();

    const auto &msgs = subscription.get_messages();

    EXPECT_EQ(msgs.size(), 1);
    EXPECT_EQ(msgs[0].header_.sender_uid_, msg.header_.sender_uid_);
    EXPECT_EQ(msgs[0].neuron_indexes_, msg.neuron_indexes_);
}


TEST(MessageBusSuite, CreateBusAndEndpointCPU)
{
    using SpikeMessage = knp::core::messaging::SpikeMessage;
    knp::core::MessageBus bus(knp::core::MessageBus::make_cpu_implementation());

    auto ep1{bus.create_endpoint()};
    auto ep2{bus.create_endpoint()};

    SpikeMessage msg{{knp::core::UID{}}, {1, 2, 3, 4, 5}};

    auto &subscription = ep2.subscribe<SpikeMessage>(knp::core::UID(), {msg.header_.sender_uid_});

    ep1.send_message(msg);
    // ID message and data message.
    EXPECT_EQ(bus.route_messages(), 2);
    ep2.receive_all_messages();

    const auto &msgs = subscription.get_messages();

    EXPECT_EQ(msgs.size(), 1);
    EXPECT_EQ(msgs[0].header_.sender_uid_, msg.header_.sender_uid_);
    EXPECT_EQ(msgs[0].neuron_indexes_, msg.neuron_indexes_);
}


TEST(MessageBusSuite, SynapticImpactMessageSend)
{
    using SynapticImpactMessage = knp::core::messaging::SynapticImpactMessage;
    knp::core::MessageBus bus;

    auto ep1{bus.create_endpoint()};
    knp::synapse_traits::OutputType synapse_type = knp::synapse_traits::OutputType::EXCITATORY;
    SynapticImpactMessage msg{
        {knp::core::UID{}},
        knp::core::UID{},
        knp::core::UID{},
        {{1, 2, synapse_type, 3, 4}, {4, 3, synapse_type, 2, 1}, {7, 8, synapse_type, 9, 10}}};

    auto &subscription = ep1.subscribe<SynapticImpactMessage>(knp::core::UID(), {msg.header_.sender_uid_});

    ep1.send_message(msg);
    // ID message and data message.
    EXPECT_EQ(bus.route_messages(), 2);
    ep1.receive_all_messages();

    const auto &msgs = subscription.get_messages();

    EXPECT_EQ(msgs.size(), 1);
    EXPECT_EQ(msgs[0].header_.sender_uid_, msg.header_.sender_uid_);
    ASSERT_EQ(msgs[0].presynaptic_population_uid_, msg.presynaptic_population_uid_);
    ASSERT_EQ(msgs[0].postsynaptic_population_uid_, msg.postsynaptic_population_uid_);
    ASSERT_EQ(msgs[0].impacts_, msg.impacts_);
}
