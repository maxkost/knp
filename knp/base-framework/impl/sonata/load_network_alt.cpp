//
// Created by an_vartenkov on 29.02.24.
//

#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/framework/network.h>

#include <simdjson.h>
#include <spdlog/spdlog.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>

#include <highfive/highfive.hpp>

#include "csv_content.h"
#include "save_load_network.h"
#include "type_id_defines.h"

namespace knp::framework
{
namespace fs = std::filesystem;


// int find_type_row(const CsvContent &content, int type_id)
//{
//     size_t rows = content.get_rc_size().first;
//     for (size_t i = 0; i < rows; ++i)
//     {
//         if (content.get_value<int>(i, "type") == type_id)
//             return i;
//     }
//     return -1;
// }


std::vector<std::string> get_projection_names(const HighFive::File &file)
{
    auto edges_group = file.getGroup("edges");
    size_t num_projections = edges_group.getNumberObjects();
    std::vector<std::string> result;
    result.reserve(num_projections);
    for (size_t i = 0; i < num_projections; ++i)
    {
        result.push_back(edges_group.getObjectName(i));
    }
    return result;
}


template <>
core::Projection<knp::synapse_traits::DeltaSynapse> load_projection(
    const HighFive::Group &edges_group, const std::string &projection_name)
{
    SPDLOG_DEBUG("Loading edges");
    auto projection_group = edges_group.getGroup(projection_name);
    auto group = projection_group.getGroup("0");
    size_t group_size = edges_group.getGroup(projection_name).getDataSet("edge_group_id").getDimensions().at(0);
    using SynapseParams = synapse_traits::synapse_parameters<synapse_traits::DeltaSynapse>;
    std::vector<core::Projection<synapse_traits::DeltaSynapse>::Synapse> target(group_size);

    const auto weights = read_parameter<decltype(SynapseParams::weight_)>(
        group, "syn_weight", group_size, synapse_traits::default_values<synapse_traits::DeltaSynapse>::weight_);
    const auto delays = read_parameter<decltype(SynapseParams::delay_)>(
        group, "delay", group_size, synapse_traits::default_values<synapse_traits::DeltaSynapse>::delay_);
    const auto out_types = read_parameter<int>(
        group, "output_type_", group_size,
        static_cast<size_t>(synapse_traits::default_values<synapse_traits::DeltaSynapse>::output_type_));
    const auto source_ids = read_parameter<size_t>(projection_group, "source_node_id", group_size, 0);
    const auto target_ids = read_parameter<size_t>(projection_group, "target_node_id", group_size, 0);

    core::UID uid_from{boost::lexical_cast<boost::uuids::uuid>(
        projection_group.getDataSet("source_node_id").getAttribute("node_population").read<std::string>())};
    core::UID uid_to{boost::lexical_cast<boost::uuids::uuid>(
        projection_group.getDataSet("target_node_id").getAttribute("node_population").read<std::string>())};
    core::UID uid_own{boost::lexical_cast<boost::uuids::uuid>(projection_name)};

    using SynParams = synapse_traits::synapse_parameters<synapse_traits::DeltaSynapse>;
    using Synapse = std::tuple<SynParams, size_t, size_t>;
    std::vector<Synapse> synapses;
    synapses.reserve(group_size);
    for (size_t i = 0; i < weights.size(); ++i)
    {
        synapse_traits::synapse_parameters<synapse_traits::DeltaSynapse> syn;
        syn.weight_ = weights[i];
        syn.delay_ = delays[i];
        syn.output_type_ = static_cast<synapse_traits::OutputType>(out_types[i]);
        size_t id_from_ = source_ids[i];
        size_t id_to_ = target_ids[i];
        synapses.push_back({syn, id_from_, id_to_});
    }

    core::Projection<synapse_traits::DeltaSynapse> proj(
        uid_own, uid_from, uid_to, [&synapses](size_t i) { return synapses[i]; }, synapses.size());

    if (projection_group.hasAttribute("is_locked"))
    {
        if (projection_group.getAttribute("is_locked").read<bool>())
            proj.lock_weights();
        else
            proj.unlock_weights();
    }

    return proj;
}


std::vector<core::AllProjectionsVariant> load_projections(const fs::path &proj_h5_file)
{
    if (!fs::is_regular_file(proj_h5_file)) throw std::runtime_error("Couldn't open file " + proj_h5_file.string());
    HighFive::File storage{proj_h5_file};
    auto group = storage.getGroup("edges");
    size_t num_projections = group.getNumberObjects();
    std::vector<core::AllProjectionsVariant> result;

    for (size_t i = 0; i < num_projections; ++i)
    {
        std::string proj_name = group.getObjectName(i);
        int proj_type =
            group.getGroup(proj_name).getDataSet("edge_type_id").read<std::vector<int>>()[0];  // one type only
        // TODO: Check if type in type_file
        if (proj_type == get_synapse_type_id<synapse_traits::DeltaSynapse>())
            result.emplace_back(load_projection<synapse_traits::DeltaSynapse>(group, proj_name));
        else if (proj_type == get_synapse_type_id<synapse_traits::SynapticResourceSTDPDeltaSynapse>())
            result.emplace_back(load_projection<synapse_traits::SynapticResourceSTDPDeltaSynapse>(group, proj_name));
        // TODO: Add other supported types or better use a template!
    }
    return result;
}


std::vector<core::AllPopulationsVariant> load_populations(const fs::path &pop_h5_file)
{
    if (!fs::is_regular_file(pop_h5_file)) throw std::runtime_error("Couldn't open file " + pop_h5_file.string());
    HighFive::File storage{pop_h5_file};
    std::cout << storage.getNumberObjects() << std::endl;
    auto group = storage.getGroup("nodes");
    size_t num_projections = group.getNumberObjects();
    std::vector<core::AllPopulationsVariant> result;

    for (size_t i = 0; i < num_projections; ++i)
    {
        std::string proj_name = group.getObjectName(i);
        int proj_type =
            group.getGroup(proj_name).getDataSet("node_type_id").read<std::vector<int>>()[0];  // one type only
        // Check if type in type_file
        if (proj_type == get_neuron_type_id<neuron_traits::BLIFATNeuron>())
            result.emplace_back(load_population<neuron_traits::BLIFATNeuron>(group, proj_name));
        else if (proj_type == get_neuron_type_id<neuron_traits::SynapticResourceSTDPBLIFATNeuron>())
            result.emplace_back(load_population<neuron_traits::SynapticResourceSTDPBLIFATNeuron>(group, proj_name));
        // TODO: Add other supported types or better use a template!
    }
    return result;
}


struct NetworkConfig
{
    const fs::path config_path;
    const fs::path edges_storage;
    const fs::path nodes_storage;
    const fs::path edges_types;
    const fs::path nodes_types;
};


NetworkConfig read_config_file(const fs::path &config_path)
{
    fs::path network_dir = config_path.parent_path();
    fs::path edges_storage = network_dir / "projections.h5";
    fs::path nodes_storage = network_dir / "populations.h5";
    fs::path edges_types = network_dir / "synapses.csv";
    fs::path nodes_types = network_dir / "neurons.csv";
    return NetworkConfig{
        .config_path{config_path},
        .edges_storage{edges_storage},
        .nodes_storage{nodes_storage},
        .edges_types{edges_types},
        .nodes_types{nodes_types}};
    // TODO: actually read config
}


core::UID get_network_uid(const fs::path &nodes_path)
{
    HighFive::File h5_file(nodes_path);
    if (h5_file.hasAttribute("network_uid"))
    {
        auto uid_str = h5_file.getAttribute("network_uid").read<std::string>();
        return core::UID{boost::lexical_cast<boost::uuids::uuid>(uid_str)};
    }
    else
        return core::UID{};
}


Network load_network_alt(const fs::path &config_path)
{
    // Open and read config files
    auto config = read_config_file(config_path);
    Network network{get_network_uid(config.nodes_storage)};
    auto populations = load_populations(config.nodes_storage);
    auto projections = load_projections(config.edges_storage);
    for (auto &pop : populations) std::visit([&network](auto &population) { network.add_population(population); }, pop);
    for (auto &proj : projections)
        std::visit([&network](auto &projection) { network.add_projection(projection); }, proj);

    // TODO Load network ID too!
    return network;
};

}  // namespace knp::framework
