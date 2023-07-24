/**
 * Output channels and converters tests.
 */

#include <knp/core/message_bus.h>
#include <knp/framework/io/out_converters/convert_bitwise.h>
#include <knp/framework/io/out_converters/convert_count.h>
#include <knp/framework/io/out_converters/convert_set.h>
#include <knp/framework/io/output_channel.h>

#include <tests_common.h>

#include <set>
#include <vector>


TEST(OutputSuite, ConverterTest)
{
    knp::core::UID sender_uid;
    // All indexes over 7 should be ignored.
    knp::core::messaging::SpikeMessage msg_0{{sender_uid, 1}, {1, 3, 8}};
    knp::core::messaging::SpikeMessage msg_1{{sender_uid, 3}, {1, 4, 10}};
    knp::core::messaging::SpikeMessage msg_2{{sender_uid, 5}, {1, 4, 7, 12}};

    std::vector<knp::core::messaging::SpikeMessage> messages{msg_0, msg_1, msg_2};

    auto result_count = knp::framework::output::converter_count(messages, 8);
    decltype(result_count) expected_count{0, 3, 0, 1, 2, 0, 0, 1};
    ASSERT_EQ(result_count, expected_count);

    auto bitwise_result = knp::framework::output::converter_bitwise(messages, 8);
    decltype(bitwise_result) expected_bitwise{false, true, false, true, true, false, false, true};
    ASSERT_EQ(bitwise_result, expected_bitwise);

    auto set_result = knp::framework::output::ConvertToSet(8)(messages);
    decltype(set_result) expected_set{1, 3, 4, 7};
    ASSERT_EQ(set_result, expected_set);
}


TEST(OutputSuite, ChannelTest)
{
    knp::core::MessageBus bus;
    knp::core::MessageEndpoint endpoint = bus.create_endpoint();
    // New random UID.
    knp::core::UID sender_uid;

    const size_t out_size = 8;
    auto count_converter = [](const std::vector<knp::core::messaging::SpikeMessage> &messages)
    { return knp::framework::output::converter_count(messages, out_size); };
    auto set_converter = knp::framework::output::ConvertToSet(out_size);
    auto max_converter = [&](const std::vector<knp::core::messaging::SpikeMessage> &messages) -> size_t
    {
        auto result_container = count_converter(messages);
        return std::max_element(result_container.begin(), result_container.end()) - result_container.begin();
    };

    // Initialize counting channel
    knp::framework::output::OutputChannel<std::vector<size_t>> channel_count{
        std::move(bus.create_endpoint()), count_converter, sender_uid};

    // Initialize neuron set channel
    knp::framework::output::OutputChannel<std::set<knp::core::messaging::SpikeIndex>> channel_set{
        std::move(bus.create_endpoint()), set_converter, sender_uid};

    // Create a custom "max activations by neuron" channel
    knp::framework::output::OutputChannel<size_t> channel_max{
        std::move(bus.create_endpoint()), max_converter, sender_uid};

    //
    // Do message exchange.
    //

    // Will ignore this.
    knp::core::messaging::SpikeMessage msg_0{{sender_uid, 0}, {0, 1, 2, 3, 4, 5}};
    // All indexes over 7 should also be ignored.
    knp::core::messaging::SpikeMessage msg_1{{sender_uid, 1}, {1, 3, 8}};
    knp::core::messaging::SpikeMessage msg_2{{sender_uid, 3}, {1, 4, 10}};
    knp::core::messaging::SpikeMessage msg_3{{sender_uid, 5}, {1, 4, 7, 12}};
    // Will ignore this.
    knp::core::messaging::SpikeMessage msg_4{{sender_uid, 6}, {1, 2, 4, 7, 10}};
    endpoint.send_message(msg_0);
    endpoint.send_message(msg_1);
    endpoint.send_message(msg_2);
    endpoint.send_message(msg_3);
    bus.route_messages();
    endpoint.receive_all_messages();

    // Use channels
    std::vector<size_t> count_result;
    decltype(count_result) expected_count{0, 3, 0, 1, 2, 0, 0, 1};  // Expected result
    count_result = channel_count.get(1, 5);

    std::set<knp::core::messaging::SpikeIndex> set_result;
    decltype(set_result) expected_set{1, 3, 4, 7};  // Expected result
    set_result = channel_set.get(1, 5);

    size_t index;
    decltype(index) expected_index = 1;  // Expected result
    index = channel_max.get(1, 5);

    // Compare with expected
    ASSERT_EQ(count_result, expected_count);
    ASSERT_EQ(set_result, expected_set);
    ASSERT_EQ(index, expected_index);
}
