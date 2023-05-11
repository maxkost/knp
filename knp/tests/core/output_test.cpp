//
// Output channels and converters tests.
//

#include <knp/core/message_bus.h>
#include <knp/core/output_channel.h>
#include <knp/core/output_converter.h>

#include <tests_common.h>

#include <set>
#include <vector>


TEST(OutputSuite, ConverterTest)
{
    knp::core::UID sender_uid;
    knp::core::messaging::SpikeMessage msg_0{{sender_uid, 0}, {0, 1, 2, 3, 4, 5}};  // Will ignore this.
    knp::core::messaging::SpikeMessage msg_1{{sender_uid, 1}, {1, 3, 8}};  // All indexes over 7 should also be ignored
    knp::core::messaging::SpikeMessage msg_2{{sender_uid, 3}, {1, 4, 10}};
    knp::core::messaging::SpikeMessage msg_3{{sender_uid, 5}, {1, 4, 7, 12}};

    std::vector<knp::core::messaging::SpikeMessage> messages{msg_0, msg_1, msg_2, msg_3};

    auto result_count = knp::core::output::converter_count(messages, 8, 1);
    decltype(result_count) expected_count{0, 3, 0, 1, 2, 0, 0, 1};
    ASSERT_EQ(result_count, expected_count);

    auto bitwise_result = knp::core::output::converter_bitwise(messages, 8, 1);
    decltype(bitwise_result) expected_bitwise{false, true, false, true, true, false, false, true};
    ASSERT_EQ(bitwise_result, expected_bitwise);

    auto set_result = knp::core::output::converter_to_set(messages, 8, 1);
    decltype(set_result) expected_set{1, 3, 4, 7};
    ASSERT_EQ(set_result, expected_set);
}


TEST(OutputSuite, ChannelTest)
{
    knp::core::MessageBus bus;
    knp::core::MessageEndpoint endpoint = bus.create_endpoint();
    knp::core::UID sender_uid;  // New random UID.

    const size_t out_size = 8;

    // Initialize counting channel
    knp::core::output::OutputChannel<std::vector<size_t>> channel_count{
        endpoint, knp::core::output::converter_count, sender_uid, out_size};
    channel_count.set_cutoff_step(1);

    // Initialize neuron set channel
    knp::core::output::OutputChannel<std::set<knp::core::messaging::SpikeIndexType>> channel_set{
        endpoint, knp::core::output::converter_to_set, sender_uid, out_size};
    channel_set.set_cutoff_step(1);

    // Create a custom "max activations by neuron" channel
    knp::core::output::OutputConverter<knp::core::messaging::SpikeIndexType> converter_max =
        [](const std::vector<knp::core::messaging::SpikeMessage> &msgs, size_t output_size,
           knp::core::messaging::TimeType cutoff_step) -> size_t
    {
        auto values = knp::core::output::converter_count(msgs, output_size, cutoff_step);
        return std::max_element(values.begin(), values.end()) - values.begin();
    };
    knp::core::output::OutputChannel<size_t> channel_max{endpoint, converter_max, sender_uid, out_size};
    channel_max.set_cutoff_step(1);

    // Do message exchange.
    knp::core::messaging::SpikeMessage msg_0{{sender_uid, 0}, {0, 1, 2, 3, 4, 5}};  // Will ignore this.
    knp::core::messaging::SpikeMessage msg_1{{sender_uid, 1}, {1, 3, 8}};  // All indexes over 7 should also be ignored
    knp::core::messaging::SpikeMessage msg_2{{sender_uid, 3}, {1, 4, 10}};
    knp::core::messaging::SpikeMessage msg_3{{sender_uid, 5}, {1, 4, 7, 12}};
    endpoint.send_message(msg_0);
    endpoint.send_message(msg_1);
    endpoint.send_message(msg_2);
    endpoint.send_message(msg_3);
    bus.route_messages();
    endpoint.receive_all_messages();

    // Use channels
    std::vector<size_t> count_result;
    decltype(count_result) expected_count{0, 3, 0, 1, 2, 0, 0, 1};  // Expected result
    channel_count >> count_result;

    std::set<knp::core::messaging::SpikeIndexType> set_result;
    decltype(set_result) expected_set{1, 3, 4, 7};  // Expected result
    channel_set >> set_result;

    size_t index;
    decltype(index) expected_index = 1;  // Expected result
    channel_max >> index;

    // Compare with expected
    ASSERT_EQ(count_result, expected_count);
    ASSERT_EQ(set_result, expected_set);
    ASSERT_EQ(index, expected_index);
}
