/**
 * @file data_storage.cpp
 * @brief File for saving and loading data.
 * @author An. Vartenkov
 * @date 16.04.2024
 */

#include <knp/framework/io/data_storage.h>

#include <hdf5.h>

#include <filesystem>

#include "sonata/highfive.h"


namespace knp::framework
{
namespace fs = std::filesystem;

constexpr int MAGIC_NUMBER = 2682;

std::vector<size_t> from_spikes_to_data(
    const std::vector<core::messaging::SpikeMessage> &message_list, size_t output_size)
{
    std::vector<size_t> result(output_size, 0);
    for (const auto &msg : message_list)
    {
        for (const auto index : msg.neuron_indexes_)
        {
            if (index <= output_size) ++result[index];
        }
    }
    return result;
}


std::vector<core::messaging::SpikeMessage> load_messages_from_h5(
    const fs::path &path_to_h5, const knp::core::UID &uid, float time_per_step)
{
    using Message = knp::core::messaging::SpikeMessage;
    HighFive::File h5_file(path_to_h5);

    // Check magic number
    int magic_number = h5_file.getAttribute("magic").read<int>();
    if (magic_number != MAGIC_NUMBER)
        throw std::runtime_error(
            "Wrong magic number " + std::to_string(magic_number) + ". It should be " + std::to_string(MAGIC_NUMBER));

    // File should have "spikes" group.
    std::vector<std::string> obj_names = h5_file.listObjectNames();
    if (std::find(obj_names.begin(), obj_names.end(), std::string("spikes")) == obj_names.end())
        throw std::runtime_error("No \"spikes\" group in file " + path_to_h5.string());
    auto data_group = h5_file.getGroup("spikes");

    // Check if "internal" needed
    obj_names = data_group.listObjectNames();
    if (std::find(obj_names.begin(), obj_names.end(), std::string("internal")) != obj_names.end())
    {
        data_group = data_group.getGroup("internal");
        obj_names = data_group.listObjectNames();
    }
    // Find "gids" or "node_ids" group inside "spikes" or "spikes/internal"
    std::string node_name;
    if (std::find(obj_names.begin(), obj_names.end(), std::string("node_ids")) != obj_names.end())
        node_name = "node_ids";
    else if (std::find(obj_names.begin(), obj_names.end(), std::string("gids")) != obj_names.end())
        node_name = "gids";
    else
        throw std::runtime_error(R"--(Neither "gids" nor "node_ids" found in data file.)--");

    // Check that timestamp dataset exists.
    if (std::find(obj_names.begin(), obj_names.end(), std::string("timestamps")) == obj_names.end())
        throw std::runtime_error(R"--(Couldn't find "timestamps" dataset in data file.)--");

    // Load datasets
    const auto node_dataset = data_group.getDataSet(node_name);
    const auto timestamps_dataset = data_group.getDataSet("timestamps");

    // They must have the same size
    if (timestamps_dataset.getElementCount() != node_dataset.getElementCount())
        throw std::runtime_error("Different number of elements in node and timestamp datasets.");

    // Read data from datasets to vectors
    std::vector<float> timestamps(timestamps_dataset.getElementCount());
    timestamps_dataset.read(timestamps);
    std::vector<int64_t> nodes(node_dataset.getElementCount());
    node_dataset.read(nodes);

    // Using unordered map: if the file is sorted by node ID, it still allows inserting messages by step in O(1)
    std::unordered_map<size_t, Message> message_map;  // Result buffer
    for (size_t i = 0; i < timestamps.size(); ++i)
    {
        auto step = static_cast<core::Step>(timestamps[i] / time_per_step);
        auto index = nodes[i];
        auto iterator = message_map.find(step);
        if (iterator == message_map.end())
        {
            Message message{{uid, step}, std::vector<core::messaging::SpikeIndex>(1, index)};
            message_map.insert({step, std::move(message)});
        }
        else
            iterator->second.neuron_indexes_.push_back(index);
    }

    // move messages from map to vector, O(N_messages)
    std::vector<Message> result;
    result.reserve(message_map.size());
    std::transform(
        message_map.begin(), message_map.end(), std::back_inserter(result),
        [](auto &val) { return std::move(val.second); });

    // Sort message vector by message step.
    std::sort(
        result.begin(), result.end(),
        [](const Message &msg1, const Message &msg2) { return msg1.header_.send_time_ < msg2.header_.send_time_; });

    return result;
}


void save_messages_to_h5(
    std::vector<core::messaging::SpikeMessage> messages, const std::filesystem::path &path_to_save, float time_per_step)
{
    HighFive::File data_file(path_to_save, HighFive::File::Create | HighFive::File::Overwrite);

    // Create base attributes
    data_file.createAttribute("magic", MAGIC_NUMBER);
    data_file.createAttribute("version", std::array<int, 2>{0, 1});
    auto spike_group = data_file.createGroup("spikes");
    spike_group.createAttribute("sorting", "by_timestamps");

    // Calculate total number of spikes
    auto add_size = [](size_t sum, const auto &msg) { return sum + msg.neuron_indexes_.size(); };
    size_t total_size = std::accumulate(messages.begin(), messages.end(), size_t{0}, add_size);

    // Reserve dataset vectors
    std::vector<float> timestamps;
    std::vector<int64_t> nodes;
    timestamps.reserve(total_size);
    nodes.reserve(total_size);

    // Sort messages by step (dataset is "sorted by timestamp")
    std::sort(
        messages.begin(), messages.end(),
        [](const core::messaging::SpikeMessage &msg1, const core::messaging::SpikeMessage &msg2)
        { return msg1.header_.send_time_ < msg2.header_.send_time_; });

    // Form dataset vectors
    for (const auto &msg : messages)
    {
        const std::vector<float> values(
            msg.neuron_indexes_.size(), static_cast<float>(msg.header_.send_time_) * time_per_step);
        timestamps.insert(timestamps.end(), values.begin(), values.end());
        nodes.insert(nodes.end(), msg.neuron_indexes_.begin(), msg.neuron_indexes_.end());
    }

    // Create datasets
    spike_group.createDataSet("node_ids", nodes);
    spike_group.createDataSet("timestamps", timestamps).createAttribute("units", "ms");
}

}  // namespace knp::framework
