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
    knp::core::MessageBus bus = knp::core::MessageBus::construct_bus();
    auto endpoint = bus.create_endpoint();
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
    auto endpoint_1 = bus.create_endpoint();
    auto c1_uid = knp::core::UID();
    endpoint_1.subscribe<knp::core::messaging::SpikeMessage>(c1_uid, {sender_uid});
    knp::framework::output::OutputChannel channel_count{c1_uid, std::move(endpoint_1)};

    // Initialize neuron set channel
    auto endpoint_2 = bus.create_endpoint();
    auto c2_uid = knp::core::UID();
    endpoint_2.subscribe<knp::core::messaging::SpikeMessage>(c2_uid, {sender_uid});
    knp::framework::output::OutputChannel channel_set{c2_uid, std::move(endpoint_2)};

    // Create a custom "max activations by neuron" channel
    auto endpoint_3 = bus.create_endpoint();
    auto c3_uid = knp::core::UID();
    endpoint_3.subscribe<knp::core::messaging::SpikeMessage>(c3_uid, {sender_uid});
    knp::framework::output::OutputChannel channel_max{c3_uid, std::move(endpoint_3)};

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
    std::vector<size_t> count_result =
        knp::framework::output::output_channel_get<std::vector<size_t>>(channel_count, count_converter, 1, 5);
    decltype(count_result) expected_count{0, 3, 0, 1, 2, 0, 0, 1};  // Expected result

    std::set<knp::core::messaging::SpikeIndex> set_result =
        knp::framework::output::output_channel_get<std::set<knp::core::messaging::SpikeIndex>>(
            channel_set, set_converter, 1, 5);
    decltype(set_result) expected_set{1, 3, 4, 7};  // Expected result

    size_t index = knp::framework::output::output_channel_get<size_t>(channel_max, max_converter, 1, 5);
    decltype(index) expected_index = 1;  // Expected result

    // Compare with expected
    ASSERT_EQ(count_result, expected_count);
    ASSERT_EQ(set_result, expected_set);
    ASSERT_EQ(index, expected_index);
}
