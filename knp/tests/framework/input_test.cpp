//
// Input channels and converters tests.
//

#include "knp/core/message_bus.h"
#include "knp/core/messaging/messaging.h"
#include "knp/framework/io/input_channel.h"
#include "tests_common.h"


TEST(InputSuite, ConverterTest)
{
    std::stringstream stream;
    // float values equivalent to (0 1 1 0 0 1 1 0 1 0)
    stream << "0.7 1.1 1.0 -0.2 0.1 3 2 0.7 11 -1";
    knp::framework::input::SequenceConverter<float> converter(
        knp::framework::input::interpret_with_threshold<float>(1.0f), 10);

    auto result = converter(stream);
    knp::core::messaging::SpikeData expected{1, 2, 5, 6, 8};

    ASSERT_EQ(result, expected);
}


TEST(InputSuite, ChannelTest)
{
    knp::core::MessageBus bus;
    auto endpoint = bus.create_endpoint();

    auto converter = knp::framework::input::SequenceConverter<int>{knp::framework::input::interpret_as_bool<int>, 10};
    knp::framework::input::InputChannel channel{
        std::make_unique<std::stringstream>(), bus.create_endpoint(), converter};

    auto &stream = dynamic_cast<std::stringstream &>(channel.get_stream());

    // Connect to output
    knp::core::UID output_uid;
    knp::framework::input::connect_input(channel, endpoint, output_uid);

    // Send data to stream: 12 integers, a test that the final ones don't get into the message
    stream << "1 0 1 1 0 1 1 1 1 0 1 1";  // 12 integers, a test that the final ones don't get into the message
    knp::core::messaging::SpikeData expected_indexes = {0, 2, 3, 5, 6, 7, 8};
    const knp::core::messaging::Step send_time = 77;

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
