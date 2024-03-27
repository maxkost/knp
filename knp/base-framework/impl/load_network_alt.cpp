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


// Read parameter values for both projections and populations
template <class Attr>
std::vector<Attr> read_attribute(
    const HighFive::Group &population_group, const std::string &attr_name, size_t pop_size, const Attr &default_value)
{
    std::vector<Attr> result(pop_size, default_value);
    if (!population_group.hasAttribute(attr_name)) return result;

    population_group.getAttribute(attr_name).read(result);
    return result;
}


template <class Neuron>
core::Population<Neuron> load_population(const HighFive::Group &nodes_group, const std::string &population_name);


#define LOAD_NEURONS_ATTRIBUTE(target, neuron_type, attribute, h5_group, pop_size)                                 \
    {                                                                                                              \
        const auto values =                                                                                        \
            read_attribute(h5_group, #attribute, pop_size, neuron_traits::default_values<neuron_type>::attribute); \
        for (size_t i = 0; i < target.size(); ++i) target[i].attribute = values[i];                                \
    }

#define LOAD_SYNAPSE_ATTRIBUTE(target, synapse_type, attribute, h5_group, proj_size)                                  \
    {                                                                                                                 \
        const auto values =                                                                                           \
            read_attribute(h5_group, #attribute, proj_size, synapse_traits::default_values<synapse_type>::attribute); \
        for (size_t i = 0; i < target.size(); ++i) target[i].attribute = values[i];                                   \
    }

template <>
core::Population<neuron_traits::BLIFATNeuron> load_population<neuron_traits::BLIFATNeuron>(
    const HighFive::Group &nodes_group, const std::string &population_name)
{
    SPDLOG_DEBUG("Loading nodes");
    auto group = nodes_group.getGroup(population_name).getGroup("0");
    size_t group_size = nodes_group.getGroup(population_name).getDataSet("node_id").getDimensions().at(0);

    std::vector<core::Population<neuron_traits::BLIFATNeuron>> result;
    // TODO: Load default neuron from json file.
    std::vector<neuron_traits::neuron_parameters<neuron_traits::BLIFATNeuron>> target(group_size);
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, n_time_steps_since_last_firing_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, activation_threshold_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, threshold_decay_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, threshold_increment_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, postsynaptic_trace_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, postsynaptic_trace_decay_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, postsynaptic_trace_increment_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, inhibitory_conductance_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, inhibitory_conductance_decay_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, potential_decay_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, bursting_period_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, reflexive_weight_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, reversal_inhibitory_potential_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, absolute_refractory_period_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, potential_reset_value_, group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, min_potential_, group, group_size)

    // Dynamic
    auto dyn_group = group.getGroup("dynamics_params");
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, dynamic_threshold_, dyn_group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, potential_, dyn_group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, pre_impact_potential_, dyn_group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, bursting_phase_, dyn_group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, total_blocking_period_, dyn_group, group_size)
    LOAD_NEURONS_ATTRIBUTE(target, neuron_traits::BLIFATNeuron, dopamine_value_, dyn_group, group_size)

    knp::core::UID uid{boost::lexical_cast<boost::uuids::uuid>(population_name)};
    core::Population<neuron_traits::BLIFATNeuron> out_population(
        uid, [&target](size_t i) { return target[i]; }, group_size);
    return out_population;
}


template <class Synapse>
core::Projection<Synapse> load_projection(const HighFive::Group &edges_group, const std::string &projection_name);


template <>
core::Projection<knp::synapse_traits::DeltaSynapse> load_projection(
    const HighFive::Group &edges_group, const std::string &projection_name)
{
    SPDLOG_DEBUG("Loading edges");
    auto projection_group = edges_group.getGroup(projection_name);
    auto group = projection_group.getGroup("0");
    size_t group_size = edges_group.getGroup(projection_name).getDataSet("edge_group_id").getDimensions().at(0);

    std::vector<core::Projection<synapse_traits::DeltaSynapse>::Synapse> target(group_size);

    const auto weights = read_attribute<double>(
        group, "syn_weight", group_size, synapse_traits::default_values<synapse_traits::DeltaSynapse>::weight_);
    const auto delays = read_attribute<size_t>(
        group, "delay", group_size, synapse_traits::default_values<synapse_traits::DeltaSynapse>::delay_);
    const auto out_types = read_attribute<size_t>(
        group, "output_type_", group_size,
        static_cast<size_t>(synapse_traits::default_values<synapse_traits::DeltaSynapse>::output_type_));
    const auto source_ids = read_attribute<size_t>(projection_group, "source_node_id", group_size, 0);
    const auto target_ids = read_attribute<size_t>(projection_group, "target_node_id", group_size, 0);

    // TEMP
    //    auto dataset = projection_group.getDataSet("source_node_id");
    //    std::cout << dataset.getNumberAttributes() << std::endl;
    //    for (auto &name : dataset.listAttributeNames())
    //        std::cout << name << " ";
    //    std::cout << std::endl;

    // END TEMP
    core::UID uid_from{boost::lexical_cast<boost::uuids::uuid>(
        projection_group.getDataSet("source_node_id").getAttribute("node_population").read<std::string>())};
    core::UID uid_to{boost::lexical_cast<boost::uuids::uuid>(
        projection_group.getDataSet("target_node_id").getAttribute("node_population").read<std::string>())};
    core::UID uid_own{boost::lexical_cast<boost::uuids::uuid>(projection_name)};

    using SynParams = synapse_traits::synapse_parameters<synapse_traits::DeltaSynapse>;
    using Synapse = std::tuple<SynParams, size_t, size_t>;
    std::vector<Synapse> synapses;
    synapses.reserve(group_size);
    for (size_t i = 0; i < synapses.size(); ++i)
    {
        synapse_traits::synapse_parameters<synapse_traits::DeltaSynapse> syn;
        syn.weight_ = weights[i];
        syn.delay_ = delays[i];
        syn.output_type_ = static_cast<synapse_traits::OutputType>(out_types[i]);
        size_t id_from_ = source_ids[i];
        size_t id_to_ = target_ids[i];
        synapses.push_back(std::make_tuple(syn, id_from_, id_to_));
    }

    core::Projection<synapse_traits::DeltaSynapse> proj(
        uid_own, uid_from, uid_to, [&synapses](size_t i) { return synapses[i]; }, synapses.size());
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
        // Check if type in type_file
        if (proj_type == get_synapse_type_id<synapse_traits::DeltaSynapse>())
            result.push_back(load_projection<synapse_traits::DeltaSynapse>(group, proj_name));
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
            result.push_back(load_population<neuron_traits::BLIFATNeuron>(group, proj_name));
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


Network load_network_alt(const fs::path &config_path)
{
    // Open and read config files
    auto config = read_config_file(config_path);
    Network network;
    auto populations = load_populations(config.nodes_storage);
    auto projections = load_projections(config.edges_storage);
    for (auto &pop : populations) std::visit([&network](auto &population) { network.add_population(population); }, pop);
    for (auto &proj : projections)
        std::visit([&network](auto &projection) { network.add_projection(projection); }, proj);

    // TODO Load network ID too!
    return network;
};

}  // namespace knp::framework
