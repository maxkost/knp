//
// Created by an_vartenkov on 31.01.24.
//

#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/framework/network.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/neuron-traits/stdp_type_traits.h>
#include <knp/synapse-traits/delta.h>

#include <spdlog/spdlog.h>

#include <filesystem>

#include <boost/format.hpp>

#include <highfive/H5File.hpp>

#include "csv_content.h"
#include "save_load_network.h"
#include "type_id_defines.h"

namespace knp::framework
{
namespace fs = std::filesystem;

template <>
void add_projection_to_h5<core::Projection<synapse_traits::DeltaSynapse>>(
    HighFive::File &file_h5, const knp::core::Projection<synapse_traits::DeltaSynapse> &projection)
{
    using SynapseParams = synapse_traits::synapse_parameters<synapse_traits::DeltaSynapse>;

    if (!file_h5.exist("edges")) throw std::runtime_error("File doesn't contain \"edges\" group.");

    std::vector<uint64_t> source_ids, target_ids;
    std::vector<decltype(SynapseParams::delay_)> delays;
    std::vector<decltype(SynapseParams::weight_)> weights;
    std::vector<int> out_types;
    source_ids.reserve(projection.size());
    target_ids.reserve(projection.size());
    delays.reserve(projection.size());
    weights.reserve(projection.size());
    out_types.reserve(projection.size());

    for (const auto &v : projection)
    {
        source_ids.push_back(std::get<1>(v));
        target_ids.push_back(std::get<2>(v));
        delays.push_back(std::get<0>(v).delay_);
        weights.push_back(std::get<0>(v).weight_);
        out_types.push_back(static_cast<int>(std::get<0>(v).output_type_));
    }

    HighFive::Group proj_group = file_h5.createGroup("edges/" + std::string(projection.get_uid()));
    HighFive::DataSet source_node_dataset = proj_group.createDataSet("source_node_id", source_ids);
    source_node_dataset.createAttribute("node_population", std::string(projection.get_presynaptic()));

    HighFive::DataSet target_node_dataset = proj_group.createDataSet("target_node_id", target_ids);
    target_node_dataset.createAttribute("node_population", std::string(projection.get_postsynaptic()));

    // Right now we only support one synapse group.
    proj_group.createDataSet("edge_group_id", std::vector(projection.size(), 0));
    proj_group.createDataSet(
        "edge_type_id", std::vector(projection.size(), get_synapse_type_id<synapse_traits::DeltaSynapse>()));

    std::vector<uint64_t> group_index;
    group_index.reserve(projection.size());
    for (size_t i = 0; i < projection.size(); ++i) group_index.push_back(i);
    proj_group.createDataSet("edge_group_index", group_index);

    HighFive::Group syn_group = proj_group.createGroup("0");
    syn_group.createDataSet("syn_weight", weights);
    syn_group.createDataSet("delay", delays);
    syn_group.createDataSet("output_type_", out_types);
}


template <>
void add_synapse_type_to_csv<synapse_traits::DeltaSynapse>(const fs::path &csv_path)
{
    CsvContent csv_file;
    if (fs::is_regular_file(csv_path))
    {
        // File already exists, load it.
        csv_file.load(csv_path);
        // Check header correctness.
        auto file_header = csv_file.get_header();
        for (const auto &column_name : edge_file_header)
        {
            if (std::find(file_header.begin(), file_header.end(), column_name) == file_header.end())
                throw std::runtime_error("Couldn't find column: " + column_name + " in file " + csv_path.string());
        }
        // Header is okay, check if type exists already.
        size_t height = csv_file.get_rc_size().first - 1;
        for (size_t row_id = 0; row_id < height; ++row_id)
        {
            int type_id = csv_file.get_value<int>(row_id, "edge_type_id");
            if (type_id == get_synapse_type_id<synapse_traits::DeltaSynapse>())
            {
                return;  // Type exists, nothing to add. TODO: upddate
            }
        }
        std::vector<std::string> type_row{
            std::to_string(get_synapse_type_id<synapse_traits::DeltaSynapse>()), "",
            get_synapse_type_name<synapse_traits::DeltaSynapse>()};
        csv_file.add_row(type_row);
        csv_file.save(csv_path);
        return;
    }
    // File doesn't exist.
    csv_file.set_header(edge_file_header);
    std::vector<std::string> type_row{
        std::to_string(get_synapse_type_id<synapse_traits::DeltaSynapse>()), "",
        get_synapse_type_name<synapse_traits::DeltaSynapse>()};
    csv_file.add_row(type_row);
    csv_file.save(csv_path);
}


// template <>
// void add_neuron_type_to_csv<VirtualNeuron>(const fs::path &csv_path)
//{
//     // Add virtual neurons
//     CsvContent csv_file;
//     if (fs::is_regular_file(csv_path))
//     {
//         // File already exists, load it.
//         csv_file.load(csv_path);
//         // Check header correctness.
//         auto file_header = csv_file.get_header();
//         for (const auto &column_name : node_file_header)
//         {
//             if (std::find(file_header.begin(), file_header.end(), column_name) == file_header.end())
//                 throw std::runtime_error("Couldn't find column: " + column_name + " in file " + csv_path.string());
//         }
//         // Header is okay, check if type exists already.
//         size_t height = csv_file.get_rc_size().first - 1;
//         for (size_t row_id = 0; row_id < height; ++row_id)
//         {
//             if (csv_file.get_value<int>(row_id, "node_type_id") == get_neuron_type_id<VirtualNeuron>())
//             {
//                 return;  // Type exists, nothing to add.
//             }
//         }
//     }
//     else
//     {
//         csv_file.set_header(node_file_header);
//     }
//     // Add type
//     std::vector<std::string> type_row = {
//         std::to_string(get_neuron_type_id<VirtualNeuron>()), "virtual", "",
//         get_neuron_type_name<VirtualNeuron>()};
//     csv_file.add_row(type_row);
//     csv_file.save(csv_path);
// }


void write_base_config(const fs::path &config_dir, const fs::path &net_config_dir)
{
    auto net_config_path = net_config_dir / "network_config.json";
    std::string base_config = R"({"networks": ["%s"]})";
    boost::format base_config_formatter(base_config);
    base_config = (base_config_formatter % net_config_path.string()).str();
    std::ofstream base_config_file(config_dir / "config.json");
    base_config_file << base_config;
    base_config_file.close();
}


template <class Population>
std::string population_to_config_string(const Population &population)
{
    std::stringstream res;
    res << "\"" << population.get_uid() << R"-(": { "type" : "point_neuron" })-";
    return res.str();
}


template <class Projection>
std::string projection_to_config_string(const Projection &proj)
{
    std::stringstream res;
    res << "\"" << proj.get_uid() << "\": {}";
    return res.str();
}


std::string make_projections_list(const Network &network)
{
    std::string result;
    for (auto iter = network.begin_projections(); iter != network.end_projections(); ++iter)
    {
        std::visit(
            [&result](const auto &projection)
            {
                result += projection_to_config_string(projection);
                result += ",";
            },
            *iter);
    }
    result.resize(result.size() - 1);  // Remove last comma
    return result;
}


std::string make_populations_list(const Network &network)
{
    std::string result;
    for (auto iter = network.begin_populations(); iter != network.end_populations(); ++iter)
    {
        std::visit(
            [&result](const auto &population)
            {
                result += population_to_config_string(population);
                result += ",";
            },
            *iter);
    }
    result.resize(result.size() - 1);  // Remove last comma
    return result;
}


void write_network_config(
    const fs::path &net_config_path, const fs::path &pop_filename_h5, const fs::path &proj_filename_h5,
    const fs::path &neurons_filename_csv, const fs::path &synapse_filename_csv, const Network &network)
{
    std::string manifest = R"--("manifest": {"$NETWORK_DIR": "."})--";
    std::string networks = R"--("networks": {"nodes" : [%s], "edges": [%s]})--";
    std::string populations =
        R"--({"nodes_file": "$NETWORK_DIR/%s", "node_types_file": "$NETWORK_DIR/%s", "populations" : {%s}})--";
    std::string projections =
        R"--({"edges_file": "$NETWORK_DIR/%s", "edge_types_file": "$NETWORK_DIR/%s", "populations" : {%s}})--";
    std::string whole_file = R"--({%s, %s})--";

    boost::format format_pop(populations);
    auto pop_string =
        (format_pop % pop_filename_h5.string() % neurons_filename_csv.string() % make_populations_list(network)).str();

    boost::format format_proj(projections);
    auto proj_string =
        (format_proj % proj_filename_h5.string() % synapse_filename_csv.string() % make_projections_list(network))
            .str();

    boost::format net_format(networks);
    auto net_string = (net_format % pop_string % proj_string).str();

    boost::format res_format(whole_file);
    auto res_string = (res_format % manifest % net_string).str();

    std::ofstream file_out(net_config_path);
    file_out << res_string;
}


void save_network(const Network &network, const fs::path &dir)
{
    auto net_dir = dir / "network";
    if (!is_directory(net_dir)) fs::create_directory(net_dir);

    std::string projections_filename = "projections.h5";
    std::string synapse_type_filename = "synapses.csv";
    std::string populations_filename = "populations.h5";
    std::string neuron_type_filename = "neurons.csv";

    fs::path path_to_projections(net_dir / projections_filename);
    fs::path path_to_synapse_csv(net_dir / synapse_type_filename);
    HighFive::File h5_proj_file(path_to_projections.string(), HighFive::File::Create | HighFive::File::Overwrite);
    h5_proj_file.createGroup("edges");

    for (auto iter = network.begin_projections(); iter != network.end_projections(); ++iter)
    {
        std::visit([&h5_proj_file](const auto &projection) { add_projection_to_h5(h5_proj_file, projection); }, *iter);
        std::visit(
            [&path_to_synapse_csv](const auto &projection)
            {
                using T = std::decay_t<decltype(projection)>;
                add_synapse_type_to_csv<typename T::ProjectionSynapseType>(path_to_synapse_csv);
            },
            *iter);
    }

    fs::path path_to_populations(net_dir / populations_filename);
    fs::path path_to_neurons_csv(net_dir / neuron_type_filename);
    HighFive::File h5_pop_file(path_to_populations.string(), HighFive::File::Create | HighFive::File::Overwrite);
    h5_pop_file.createGroup("nodes");
    for (auto iter = network.begin_populations(); iter != network.end_populations(); ++iter)
    {
        std::visit([&h5_pop_file](const auto &population) { add_population_to_h5(h5_pop_file, population); }, *iter);
        std::visit(
            [&path_to_neurons_csv](const auto &population)
            {
                using T = std::decay_t<decltype(population)>;
                add_neuron_type_to_csv<typename T::PopulationNeuronType>(path_to_neurons_csv);
            },
            *iter);
    }
    h5_pop_file.createAttribute("network_uid", std::string{network.get_uid()});
    // TODO : move this inside add_population or add more neurons
    add_neuron_type_to_csv<neuron_traits::BLIFATNeuron>(path_to_neurons_csv);


    write_base_config(dir, net_dir);
    write_network_config(
        net_dir / "network_config.json", populations_filename, projections_filename, neuron_type_filename,
        synapse_type_filename, network);
}

}  // namespace knp::framework
