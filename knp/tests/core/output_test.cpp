//
// Output channels and converters tests.
//

#include <knp/core/messaging/messaging.h>
#include <knp/core/output_converter.h>

#include <tests_common.h>

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
