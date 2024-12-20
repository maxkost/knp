/**
 * @file data_storage_test.cpp
 * @brief Test saving and loading spikes.
 * @kaspersky_support A. Vartenkov
 * @date 18.04.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
        knp::core::messaging::SpikeMessage msg_buf{{uid_from, i}, {}};
        for (size_t j = 0; j < neurons; ++j)
        {
            if (distribution(engine) <= probability) msg_buf.neuron_indexes_.push_back(j);
        }
        if (!msg_buf.neuron_indexes_.empty()) result.push_back(std::move(msg_buf));
    }
    return result;
}

class SaveLoadDataSuite : public ::testing::Test
{
protected:
    void SetUp() override { messages_ = generate_random_messages(uid_, 200, 20, 0.2); }


    void TearDown() override { std::filesystem::remove(file_path_); }


    std::vector<knp::core::messaging::SpikeMessage> messages_;
    std::filesystem::path file_path_;
    knp::core::UID uid_;
};


TEST_F(SaveLoadDataSuite, JsonTest)
{
    file_path_ = "data.json";
    knp::framework::io::storage::native::save_messages_to_json(messages_, file_path_);
    ASSERT_EQ(messages_, knp::framework::io::storage::native::load_messages_from_json(file_path_, uid_));
}


TEST_F(SaveLoadDataSuite, Hdf5Test)
{
    file_path_ = "data.h5";
    knp::framework::io::storage::native::save_messages_to_h5(messages_, file_path_);
    ASSERT_EQ(messages_, knp::framework::io::storage::native::load_messages_from_h5(file_path_, uid_));
}


class WrongMagicNumberJsonSuite : public ::testing::Test
{
protected:
    void SetUp() override
    {
        messages_ = generate_random_messages(uid_, 200, 20, 0.2);
        path_to_json_ = "data.json";
        knp::framework::io::storage::native::save_messages_to_json(messages_, path_to_json_);
        std::ifstream file(path_to_json_);
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        std::string json_str = buffer.str();
        auto pos = json_str.find("2682");
        json_str.replace(pos, 4, "1234");
        std::ofstream out_file(path_to_json_);
        out_file << json_str;
    }

    void TearDown() override { std::filesystem::remove(path_to_json_); }

    std::filesystem::path path_to_json_;
    std::vector<knp::core::messaging::SpikeMessage> messages_;
    const knp::core::UID uid_;
};


TEST_F(WrongMagicNumberJsonSuite, JsonTestNonStrict)
{
    auto loaded_messages = knp::framework::io::storage::native::load_messages_from_json(path_to_json_, uid_, false);
    ASSERT_EQ(messages_, loaded_messages);
}


TEST_F(WrongMagicNumberJsonSuite, JsonTestStrict)
{
    ASSERT_ANY_THROW(knp::framework::io::storage::native::load_messages_from_json(
        path_to_json_, uid_, true));  //!OCLINT(False positive)
}


class WrongMagicNumberHdf5Suite : public ::testing::Test
{
protected:
    void SetUp() override
    {
        path_to_h5_ = "data.h5";
        messages_ = generate_random_messages(uid_, 200, 20, 0.2);
        knp::framework::io::storage::native::save_messages_to_h5(messages_, path_to_h5_);
        HighFive::File h5_file(path_to_h5_.string(), HighFive::File::ReadWrite);
        h5_file.deleteAttribute("magic");
        h5_file.createAttribute("magic", 1234);
        h5_file.flush();
    }

    void TearDown() override { std::filesystem::remove(path_to_h5_); }

    std::filesystem::path path_to_h5_;
    std::vector<knp::core::messaging::SpikeMessage> messages_;
    const knp::core::UID uid_;
};


TEST_F(WrongMagicNumberHdf5Suite, Hdf5TestNonStrict)
{
    auto loaded_messages = knp::framework::io::storage::native::load_messages_from_h5(path_to_h5_, uid_, 1.0, false);
    ASSERT_EQ(messages_, loaded_messages);
}


TEST_F(WrongMagicNumberHdf5Suite, Hdf5TestStrict)
{
    ASSERT_ANY_THROW(knp::framework::io::storage::native::load_messages_from_h5(
        path_to_h5_, uid_, 1.0, true));  //!OCLINT(False positive)
}
