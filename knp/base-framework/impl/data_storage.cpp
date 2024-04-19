/**
 * @file data_storage.cpp
 * @brief File for saving and loading data.
 * @author An. Vartenkov
 * @date 16.04.2024
 */

#include <knp/framework/io/data_storage.h>

#include <hdf5.h>
#include <simdjson.h>

#include <filesystem>
#include <fstream>

#include <boost/format.hpp>

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


std::vector<core::messaging::SpikeMessage> convert_node_time_arrays_to_messages(
    const std::vector<int64_t> &nodes, const std::vector<float> &timestamps, const knp::core::UID &uid,
    float time_per_step)
{
    if (nodes.size() != timestamps.size()) throw std::runtime_error("Different array sizes: nodes and timestamps.");
    using Message = core::messaging::SpikeMessage;
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


std::vector<core::messaging::SpikeMessage> load_messages_from_h5(
    const fs::path &path_to_h5, const knp::core::UID &uid, float time_per_step)
{
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

    return convert_node_time_arrays_to_messages(nodes, timestamps, uid, time_per_step);
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
    spike_group.createDataSet("timestamps", timestamps).createAttribute("units", "step");
}


// clang-format off // No const operator[] for simdjson document, so no const parameter.
bool is_json_has_magic(simdjson::ondemand::document &doc)  // cppcheck-suppress constParameter
// clang-format on
{
    auto attributes = doc["attributes"];
    if (attributes.error()) return false;
    bool has_magic = false;
    for (auto group : attributes)
    {
        if (group["name"] == "magic")
        {
            if (group["value"].get<int64_t>() != 2682) break;
            has_magic = true;
            break;
        }
    }
    return has_magic;
}


std::vector<core::messaging::SpikeMessage> load_messages_from_json(
    const fs::path &path_to_json, const knp::core::UID &uid)
{
    simdjson::ondemand::parser parser;
    auto simdjson_result = simdjson::padded_string::load(path_to_json.string());
    if (simdjson_result.error()) throw std::runtime_error("Cannot read file " + path_to_json.string());
    simdjson::ondemand::document doc = parser.iterate(simdjson_result);

    if (!is_json_has_magic(doc)) throw std::runtime_error("Unable to find magic number: wrong file format or version");

    auto spikes_group = doc["spikes"];
    if (spikes_group.error()) throw std::runtime_error(R"--(Unable to find "spikes" group in data file.)--");

    // Read node ids
    auto nodes_array = spikes_group["node_ids"];
    if (nodes_array.error()) throw std::runtime_error(R"--(No "node_ids" array in "spikes" group.)--");
    auto nodes_array_val = nodes_array["value"];
    if (nodes_array_val.error()) throw std::runtime_error(R"--(Missing nodes data in json data file.)--");
    auto pre_array_nodes = nodes_array_val.get_array();
    if (pre_array_nodes.error()) throw std::runtime_error("Missing node data in json data file");
    std::vector<int64_t> nodes;
    nodes.reserve(pre_array_nodes.count_elements());
    std::transform(
        pre_array_nodes.begin(), pre_array_nodes.end(), std::back_inserter(nodes),
        [](auto val) { return val.get_int64(); });  // no const reference val possible

    // Read timestamps
    auto timestamps_array = spikes_group["timestamps"];
    if (timestamps_array.error()) throw std::runtime_error(R"--(No "timestamps" array in "spikes" group.)--");
    auto times_array_val = timestamps_array["value"];
    auto pre_array_times = times_array_val.get_array();
    if (times_array_val.error()) throw std::runtime_error(R"--(Missing timestamps data in json data file.)--");
    if (pre_array_times.error()) throw std::runtime_error("Missing timestamp data in json data file");
    std::vector<float> timestamps;
    timestamps.reserve(pre_array_times.count_elements());
    std::transform(
        pre_array_times.begin(), pre_array_times.end(), std::back_inserter(timestamps),
        [](auto val) { return static_cast<float>(val.get_double()); });  // no const reference val possible


    return convert_node_time_arrays_to_messages(nodes, timestamps, uid, 1);
}


const char header_string[] =
    R"--("attributes": [
{
    "name": "magic",
            "type": {
        "class": "Integer (unsigned)",
                "size": 32,
                "endianness": "little-endian"
    },
    "value": 2682
},
{
    "name": "version",
            "shape": [2],
    "type": {
        "class": "Integer (unsigned)",
        "size": 32,
        "endianness": "little-endian"
    },
    "value": [0, 1]
}
])--";


const char spike_attributes[] =
    R"--("attributes": [
      {
        "name": "sorting",
        "type": {
          "class": "Enumeration",
          "mapping": {
            "by_id": 1,
            "by_time": 2,
            "none": 0
          }
        },
        "value": "by_time"
      }
    ])--";


const char node_structure[] =
    R"--("node_ids": {
      "shape": [%d],
      "type": {
        "class": "Integer (unsigned)",
        "size": 64,
        "endianness": "little-endian"
      },
      "value": [%s]
    })--";


const char timestamp_structure[] =
    R"--("timestamps": {
      "attributes": [
        {
          "name": "units",
          "type": {
            "class": "String",
            "charSet": "ASCII"
          },
          "value": "step"
        }
      ],
      "shape": [%d],
      "type": {
        "class": "Float",
        "endianness": "little-endian"
      },
      "value": [%s]
    })--";


const char whole_file_string[] =
    R"--(
{
    %s,
    "spikes" :
        {
            %s,
            %s,
            %s
        }
    }
)--";


void save_messages_to_json(
    std::vector<core::messaging::SpikeMessage> messages, const std::filesystem::path &path_to_save)
{
    boost::format format_nodes(node_structure);
    boost::format format_times(timestamp_structure);
    boost::format format_file(whole_file_string);
    std::ostringstream node_stream;
    std::ostringstream time_stream;
    std::sort(
        messages.begin(), messages.end(),
        [](const auto &msg1, const auto &msg2) { return msg1.header_.send_time_ < msg2.header_.send_time_; });
    size_t count = 0;
    for (const auto &msg : messages)
    {
        for (auto index : msg.neuron_indexes_)
        {
            node_stream << index << ", ";
            time_stream << msg.header_.send_time_ << ", ";
            ++count;
        }
    }
    std::string node_string = node_stream.str();
    std::string time_string = time_stream.str();
    if (node_string.size() > 2)
    {
        // remove last comma
        node_string.resize(node_string.size() - 2);
        time_string.resize(time_string.size() - 2);
    }
    std::string nodes_res = (format_nodes % count % node_string).str();
    std::string times_res = (format_times % count % time_string).str();
    std::string file_string = (format_file % header_string % spike_attributes % nodes_res % times_res).str();
    std::ofstream out_file(path_to_save, std::ofstream::out);
    out_file << file_string;
}
}  // namespace knp::framework
