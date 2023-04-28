//
// Data converter tests.
//

#include <knp/core/input_converter.h>
#include <knp/core/messaging/messaging.h>

#include <tests_common.h>

TEST(InputConverterSuite, SequenceConverterTest)
{
    std::stringstream stream;
    knp::core::UID uid{true};
    std::unique_ptr<knp::core::input::DataConverter> converter{
        std::make_unique<knp::core::input::SequenceConverter<int>>(stream, uid, 10)};

    stream << "0 1 1 0 0 0 1 0 1 1 ";
    stream << "1 0 1 1 0 1 1 1 1 0 1 1";  // 12 integers, a test that the final one doesn't get into the message
    std::vector<uint32_t> expected_indexes_1 = {1, 2, 6, 8, 9};
    std::vector<uint32_t> expected_indexes_2 = {0, 2, 3, 5, 6, 7, 8, 10};
    uint64_t send_time = 777;

    knp::core::messaging::SpikeMessage message;
    message.header_.send_time_ = send_time;
    // First message
    *converter >> message;
    ASSERT_EQ(message.header_.sender_uid_, uid);
    ASSERT_EQ(message.header_.send_time_, send_time);
    ASSERT_EQ(message.neuron_indexes_, expected_indexes_1);

    // Second message
    converter->set_input_size(11);
    *converter >> message;
    ASSERT_EQ(message.header_.sender_uid_, uid);
    ASSERT_EQ(message.header_.send_time_, send_time);
    ASSERT_EQ(message.neuron_indexes_, expected_indexes_2);
}
