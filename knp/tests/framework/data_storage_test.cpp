/**
 * @file data_storage_test.cpp
 * @brief Test saving and loading spikes.
 * @author A. Vartenkov
 * @date 18.04.2024
 */

#include <knp/core/messaging/messaging.h>
#include <knp/framework/io/data_saver.h>

#include <tests_common.h>

#include <random>
#include <vector>


std::vector<knp::core::messaging::SpikeMessage> generate_random_messages(
    const knp::core::UID &uid_from, size_t steps, size_t neurons, float probability)
{
    std::uniform_real_distribution<float> distribution;
    std::mt19937 engine(0);  // NOLINT (We want this test to be predictable)

    std::vector<knp::core::messaging::SpikeMessage> result;

    for (size_t i = 0; i < steps; ++i)
    {
        knp::core::messaging::SpikeMessage msg_buf{.header_{uid_from, i}, .neuron_indexes_{}};
        for (size_t j = 0; j < neurons; ++j)
        {
            if (distribution(engine) <= probability) msg_buf.neuron_indexes_.push_back(j);
        }
        if (!msg_buf.neuron_indexes_.empty()) result.push_back(std::move(msg_buf));
    }
    return result;
}


bool is_equal_spike_message_vector(
    const std::vector<knp::core::messaging::SpikeMessage> &msg_list1,
    const std::vector<knp::core::messaging::SpikeMessage> &msg_list2)
{
    if (msg_list1.size() != msg_list2.size()) return false;
    for (size_t i = 0; i < msg_list1.size(); ++i)
    {
        if (msg_list1[i].header_.send_time_ != msg_list2[i].header_.send_time_) return false;
        if (msg_list1[i].header_.sender_uid_ != msg_list2[i].header_.sender_uid_) return false;
        if (msg_list1[i].neuron_indexes_ != msg_list2[i].neuron_indexes_) return false;
    }
    return true;
}


TEST(SaveLoadSuite, SaveLoadDataTest)
{
    knp::core::UID uid;
    auto messages = generate_random_messages(uid, 200, 20, 0.2);
    std::filesystem::path path_to_h5 = "data.h5";
    knp::framework::save_messages_to_h5(messages, path_to_h5, 1.0F);
    auto loaded_messages = knp::framework::load_messages_from_h5(path_to_h5, uid, 1.0F);
    ASSERT_TRUE(is_equal_spike_message_vector(messages, loaded_messages));
}
