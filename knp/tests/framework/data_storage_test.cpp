/**
 * @file data_storage_test.cpp
 * @brief Test saving and loading spikes.
 * @author A. Vartenkov
 * @date 18.04.2024
 */

#include <knp/core/messaging/messaging.h>
#include <knp/framework/io/storage/native/data_storage_hdf5.h>
#include <knp/framework/io/storage/native/data_storage_json.h>

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdocumentation"
#endif
#include <highfive/highfive.hpp>
#ifdef __clang__
#    pragma clang diagnostic pop
#endif

#include <tests_common.h>

#include <fstream>
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


TEST(SaveLoadSuite, JsonTest)
{
    namespace data = knp::framework::storage::native;
    knp::core::UID uid;
    auto messages = generate_random_messages(uid, 200, 20, 0.2);
    std::filesystem::path path_to_json = "data.json";
    data::save_messages_to_json(messages, path_to_json);

    // This is the normal case.
    ASSERT_EQ(messages, data::load_messages_from_json(path_to_json, uid));

    // Here we change format slightly.
    std::ifstream file(path_to_json);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::string json_str = buffer.str();
    json_str.replace(json_str.find("2682"), 4, "1234");
    std::ofstream out_file(path_to_json);
    out_file << json_str;
    ASSERT_EQ(messages, data::load_messages_from_json(path_to_json, uid, false));
    ASSERT_ANY_THROW(data::load_messages_from_json(path_to_json, uid, true));
    std::filesystem::remove(path_to_json);
}


TEST(SaveLoadSuite, Hdf5Test)
{
    namespace data = knp::framework::storage::native;
    knp::core::UID uid;
    auto messages = generate_random_messages(uid, 200, 20, 0.2);
    std::filesystem::path path_to_h5 = "data.h5";
    data::save_messages_to_h5(messages, path_to_h5);
    ASSERT_EQ(messages, data::load_messages_from_h5(path_to_h5, uid));

    // Here we change format slightly.
    HighFive::File h5_file(path_to_h5, HighFive::File::ReadWrite);
    h5_file.deleteAttribute("magic");
    h5_file.createAttribute("magic", 1234);
    h5_file.flush();

    ASSERT_EQ(messages, data::load_messages_from_h5(path_to_h5, uid, 1.0, false));
    ASSERT_ANY_THROW(data::load_messages_from_h5(path_to_h5, uid, 1.0, true));
}
