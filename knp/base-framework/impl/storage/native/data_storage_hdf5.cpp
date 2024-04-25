/**
 * @file data_storage_hdf5.cpp
 * @brief File for saving and loading data in hdf5 format.
 * @author An. Vartenkov
 * @date 16.04.2024
 */

#include <knp/framework/io/storage/native/data_storage_json.h>

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

#include "../../sonata/highfive.h"
#include "data_storage_common.h"


namespace knp::framework::storage::native
{
namespace fs = std::filesystem;


bool check_magic(const HighFive::File &h5_file, bool is_throw)
{
    if (!h5_file.hasAttribute("magic"))
    {
        if (is_throw)
            throw std::runtime_error(R"--(No "magic" attribute in file, probably wrong format.)--");
        else
            return false;
    }

    int magic_number = h5_file.getAttribute("magic").read<int>();
    if (magic_number != MAGIC_NUMBER)
    {
        if (is_throw)
        {
            throw std::runtime_error(
                "Wrong magic number " + std::to_string(magic_number) + ". It should be " +
                std::to_string(MAGIC_NUMBER));
        }
        else
            return false;
    }
    return true;
}


bool check_version(const HighFive::File &doc)
{
    if (!doc.hasAttribute("version")) return false;
    std::vector<int64_t> version;
    doc.getAttribute("version").read(version);
    return std::equal(version.begin(), version.end(), VERSION.begin());
}


std::vector<core::messaging::SpikeMessage> load_messages_from_h5(
    const fs::path &path_to_h5, const knp::core::UID &uid, float time_per_step, bool strict_format)
{
    HighFive::File h5_file(path_to_h5);


    // Checking magic number.
    if (!check_magic(h5_file, strict_format)) SPDLOG_WARN("No magic number found, probably wrong file format");

    // Checking version.
    if (!check_version(h5_file)) SPDLOG_WARN("Unable to confirm file version");

    // File should have "spikes" group.
    std::vector<std::string> obj_names = h5_file.listObjectNames();
    if (std::find(obj_names.begin(), obj_names.end(), std::string("spikes")) == obj_names.end())
        throw std::runtime_error("No \"spikes\" group in file " + path_to_h5.string());
    auto data_group = h5_file.getGroup("spikes");

    // Checking if "internal" is needed.
    obj_names = data_group.listObjectNames();
    if (std::find(obj_names.begin(), obj_names.end(), std::string("internal")) != obj_names.end())
    {
        data_group = data_group.getGroup("internal");
        obj_names = data_group.listObjectNames();
    }
    // Finding "gids" or "node_ids" group inside "spikes" or "spikes/internal".
    std::string node_name;
    if (std::find(obj_names.begin(), obj_names.end(), std::string("node_ids")) != obj_names.end())
        node_name = "node_ids";
    else if (std::find(obj_names.begin(), obj_names.end(), std::string("gids")) != obj_names.end())
        node_name = "gids";
    else
        throw std::runtime_error(R"--(Neither "gids" nor "node_ids" found in data file.)--");

    // Checking that timestamp dataset exists.
    if (std::find(obj_names.begin(), obj_names.end(), std::string("timestamps")) == obj_names.end())
        throw std::runtime_error(R"--(Couldn't find "timestamps" dataset in data file.)--");

    // Loading datasets.
    const auto node_dataset = data_group.getDataSet(node_name);
    const auto timestamps_dataset = data_group.getDataSet("timestamps");

    // They must have the same size.
    if (timestamps_dataset.getElementCount() != node_dataset.getElementCount())
        throw std::runtime_error("Different number of elements in node and timestamp datasets.");

    // Reading data from datasets to vectors.
    std::vector<float> timestamps(timestamps_dataset.getElementCount());
    timestamps_dataset.read(timestamps);
    std::vector<int64_t> nodes(node_dataset.getElementCount());
    node_dataset.read(nodes);

    return convert_node_time_arrays_to_messages(nodes, timestamps, uid, time_per_step);
}


void save_messages_to_h5(
    std::vector<core::messaging::SpikeMessage> messages, const std::filesystem::path &path_to_save, float time_per_step)
{
    HighFive::File data_file(path_to_save, HighFive::File::Create | HighFive::File::Overwrite);

    // Creating base attributes.
    data_file.createAttribute("magic", MAGIC_NUMBER);
    data_file.createAttribute("version", std::array<int, 2>{0, 1});
    auto spike_group = data_file.createGroup("spikes");
    spike_group.createAttribute("sorting", std::string{"by_timestamps"});

    // Calculating total number of spikes.
    auto add_size = [](size_t sum, const auto &msg) { return sum + msg.neuron_indexes_.size(); };
    size_t total_size = std::accumulate(messages.begin(), messages.end(), size_t{0}, add_size);

    // Reserving dataset vectors.
    std::vector<float> timestamps;
    std::vector<int64_t> nodes;
    timestamps.reserve(total_size);
    nodes.reserve(total_size);

    // Sorting messages by step (dataset is "sorted by timestamp").
    std::sort(
        messages.begin(), messages.end(),
        [](const core::messaging::SpikeMessage &msg1, const core::messaging::SpikeMessage &msg2)
        { return msg1.header_.send_time_ < msg2.header_.send_time_; });

    // Forming dataset vectors.
    for (const auto &msg : messages)
    {
        const std::vector<float> values(
            msg.neuron_indexes_.size(), static_cast<float>(msg.header_.send_time_) * time_per_step);
        timestamps.insert(timestamps.end(), values.begin(), values.end());
        nodes.insert(nodes.end(), msg.neuron_indexes_.begin(), msg.neuron_indexes_.end());
    }

    // Creating datasets.
    spike_group.createDataSet("node_ids", nodes);
    auto time_set = spike_group.createDataSet("timestamps", timestamps);
    time_set.createAttribute("units", std::string{"step"});
}

}  // namespace knp::framework::storage::native
