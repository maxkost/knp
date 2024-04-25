/**
 * @file data_storage_json.cpp
 * @brief File for saving and loading data from json format.
 * @author An. Vartenkov
 * @date 16.04.2024
 */

#include <knp/framework/io/storage/native/data_storage_json.h>

#include <simdjson.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>

#include <boost/format.hpp>

#include "data_storage_common.h"


namespace knp::framework::storage::native
{
namespace fs = std::filesystem;

constexpr char header_string[] =
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


constexpr char spike_attributes[] =
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


constexpr char node_structure[] =
    R"--("node_ids": {
  "shape": [%d],
  "type": {
    "class": "Integer (unsigned)",
    "size": 64,
    "endianness": "little-endian"
  },
  "value": [%s]
})--";


constexpr char timestamp_structure[] =
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


constexpr char whole_file_string[] =
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
            if (group["value"].get<int64_t>() != MAGIC_NUMBER) break;
            has_magic = true;
            break;
        }
    }
    return has_magic;
}


// clang-format off // No const operator[] for simdjson document, so no const parameter.
bool is_correct_version(simdjson::ondemand::document &doc)  // cppcheck-suppress constParameter
// clang-format on
{
    auto attributes = doc["attributes"];
    if (attributes.error()) return false;
    for (auto group : attributes)
    {
        if (group["name"] == "version")
        {
            auto value_group = group["value"];
            if (value_group.error()) return false;
            auto array = value_group.get_array();
            if (array.error() || VERSION.size() != array.count_elements().value()) return false;
            std::vector<int64_t> version(VERSION.size());
            std::transform(array.begin(), array.end(), version.begin(), [](auto v) { return v.get_int64(); });
            return std::equal(version.begin(), version.end(), VERSION.begin());
        }
    }
    return false;
}


std::vector<core::messaging::SpikeMessage> load_messages_from_json(
    const fs::path &path_to_json, const knp::core::UID &uid, bool strict_format)
{
    simdjson::ondemand::parser parser;
    auto simdjson_result = simdjson::padded_string::load(path_to_json.string());
    if (simdjson_result.error()) throw std::runtime_error("Cannot read file " + path_to_json.string());
    simdjson::ondemand::document doc = parser.iterate(simdjson_result);

    if (!is_json_has_magic(doc))
    {
        if (strict_format)
            throw std::runtime_error("Unable to find magic number: wrong file format or version");
        else
            SPDLOG_WARN("Unable to find magic number: wrong file format or version");
    }
    if (!is_correct_version(doc)) SPDLOG_WARN("Unable to verify file version");


    auto spikes_group = doc["spikes"];
    if (spikes_group.error()) throw std::runtime_error(R"--(Unable to find "spikes" group in data file.)--");

    // Reading node ids.
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
        [](auto val) { return val.get_int64(); });  // no const reference val possible.

    // Reading timestamps.
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
        [](auto val) { return static_cast<float>(val.get_double()); });  // no const reference val possible.


    return convert_node_time_arrays_to_messages(nodes, timestamps, uid, 1);
}


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
        // Removing last comma.
        node_string.resize(node_string.size() - 2);
        time_string.resize(time_string.size() - 2);
    }
    std::string nodes_res = (format_nodes % count % node_string).str();
    std::string times_res = (format_times % count % time_string).str();
    std::string file_string = (format_file % header_string % spike_attributes % nodes_res % times_res).str();
    std::ofstream out_file(path_to_save, std::ofstream::out);
    out_file << file_string;
}
}  // namespace knp::framework::storage::native
