//
// Input channels and converters tests.
//

#include <knp/core/input_channel.h>
#include <knp/core/message_bus.h>
#include <knp/core/messaging/messaging.h>

#include <tests_common.h>

TEST(InputSuite, ConverterTest)
{
    knp::core::MessageBus bus;

    auto endpoint{bus.create_endpoint()};
    std::stringstream stream;
    knp::core::UID uid_in{true};

    stream << "0.7 1.1 1.0 -0.2 0.1 3 2 0.7 11 -1";  // float values equivalent to (0 1 1 0 0 1 1 0 1 0)
    knp::core::input::SequenceConverter<float> converter(knp::core::input::interpret_with_threshold<float>(1.0f));

    auto result = converter(stream, 10);
    knp::core::messaging::SpikeData expected{1, 2, 5, 6, 8};

    ASSERT_EQ(result, expected);
}


TEST(InputSuite, ChannelTest)
{
    knp::core::MessageBus bus;
    auto endpoint = bus.create_endpoint();

    knp::core::input::InputChannel channel{
        endpoint, std::make_unique<std::stringstream>(), knp::core::input::SequenceConverter<int>{}, knp::core::UID{},
        10};

    auto &stream = dynamic_cast<std::stringstream &>(channel.get_stream());

    // Connect to output
    knp::core::UID output_uid;
    channel.connect(output_uid);

    // Send data to stream
    stream << "1 0 1 1 0 1 1 1 1 0 1 1";  // 12 integers, a test that the final ones don't get into the message
    std::vector<uint32_t> expected_indexes = {0, 2, 3, 5, 6, 7, 8};
    const knp::core::messaging::TimeType send_time = 77;

    // Tell channel to send a message
    channel.send(send_time);

    // Process messages
    bus.route_messages();
    endpoint.receive_all_messages();

    auto messages = endpoint.unload_messages<knp::core::messaging::SpikeMessage>(output_uid);
    ASSERT_EQ(messages.size(), 1);

    auto message = messages[0];
    ASSERT_EQ(message.header_.sender_uid_, channel.get_uid());
    ASSERT_EQ(message.header_.send_time_, send_time);
    ASSERT_EQ(message.neuron_indexes_, expected_indexes);
}
