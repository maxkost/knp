//
// Created by an_vartenkov on 29.02.24.
//

#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/framework/network.h>

#include <bbp/sonata/common.h>
#include <bbp/sonata/config.h>
#include <bbp/sonata/edges.h>
#include <bbp/sonata/nodes.h>
#include <spdlog/spdlog.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>

namespace knp::framework
{
namespace fs = std::filesystem;


template <class Neuron>
core::Population<Neuron> load_nodes(const bbp::sonata::NodePopulation& snt_population);


#define LOAD_NEURONS_ATTRIBUTE(target, attribute, sonata_population)                \
    {                                                                               \
        const auto values = sonata_population.getAttribute(                         \
            #attribute, bbp::sonata::Selection{{{0, sonata_population.size()}}},    \
            neuron_traits::default_values<neuron_traits::BLIFATNeuron>::attribute); \
        for (size_t i = 0; i < target.size(); ++i) target[i].attribute = values[i]; \
    }


#define LOAD_DYNAMIC_NEURONS_ATTRIBUTE(target, attribute, sonata_population)        \
    {                                                                               \
        const auto values = sonata_population.getDynamicsAttribute(                 \
            #attribute, bbp::sonata::Selection{{{0, sonata_population.size()}}},    \
            neuron_traits::default_values<neuron_traits::BLIFATNeuron>::attribute); \
        for (size_t i = 0; i < target.size(); ++i) target[i].attribute = values[i]; \
    }


template <>
core::Population<neuron_traits::BLIFATNeuron> load_nodes<neuron_traits::BLIFATNeuron>(
    const bbp::sonata::NodePopulation& snt_population)
{
    SPDLOG_DEBUG("Loading nodes");
    std::vector<core::Population<neuron_traits::BLIFATNeuron>> result;
    // TODO: Load default neuron from json file.
    std::vector<neuron_traits::neuron_parameters<neuron_traits::BLIFATNeuron>> target(snt_population.size());
    LOAD_NEURONS_ATTRIBUTE(target, n_time_steps_since_last_firing_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, activation_threshold_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, threshold_decay_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, threshold_increment_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, postsynaptic_trace_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, postsynaptic_trace_decay_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, postsynaptic_trace_increment_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, inhibitory_conductance_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, inhibitory_conductance_decay_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, potential_decay_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, bursting_period_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, reflexive_weight_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, reversive_inhibitory_potential_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, absolute_refractory_period_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, potential_reset_value_, snt_population)
    LOAD_NEURONS_ATTRIBUTE(target, min_potential_, snt_population)

    // Dynamic
    LOAD_DYNAMIC_NEURONS_ATTRIBUTE(target, dynamic_threshold_, snt_population)
    LOAD_DYNAMIC_NEURONS_ATTRIBUTE(target, potential_, snt_population)
    LOAD_DYNAMIC_NEURONS_ATTRIBUTE(target, pre_impact_potential_, snt_population)
    LOAD_DYNAMIC_NEURONS_ATTRIBUTE(target, bursting_phase_, snt_population)
    LOAD_DYNAMIC_NEURONS_ATTRIBUTE(target, total_blocking_period_, snt_population)
    LOAD_DYNAMIC_NEURONS_ATTRIBUTE(target, dopamine_value_, snt_population)

    knp::core::UID uid{boost::lexical_cast<boost::uuids::uuid>(snt_population.name())};
    core::Population<neuron_traits::BLIFATNeuron> out_population(
        uid, [&target](size_t i) { return target[i]; }, snt_population.size());
    return out_population;
}


template <class Synapse>
std::vector<knp::core::Projection<Synapse>> load_edges(const fs::path& edge_path);

template <>
std::vector<knp::core::Projection<knp::synapse_traits::DeltaSynapse>> load_edges(const fs::path& edge_path)
{
    bbp::sonata::EdgeStorage edges(edge_path);
    auto names = edges.populationNames();
    std::vector<knp::core::Projection<synapse_traits::DeltaSynapse>> result;
    for (auto& name : names)
    {
        auto projection = edges.openPopulation(name);
        bbp::sonata::Selection select_all{{{0, projection->size()}}};
        std::vector<core::Projection<synapse_traits::DeltaSynapse>::Synapse> synapses(projection->size());
        const auto weights = projection->getAttribute<double>("syn_weight", select_all);
        const auto delays = projection->getAttribute<size_t>("delay", select_all);
        const auto out_types = projection->getAttribute<size_t>("output_type_", select_all);
        const auto source_ids = projection->sourceNodeIDs(select_all);
        const auto target_ids = projection->targetNodeIDs(select_all);

        core::UID uid_from{boost::lexical_cast<boost::uuids::uuid>(projection->source())};
        core::UID uid_to{boost::lexical_cast<boost::uuids::uuid>(projection->target())};
        core::UID uid_own{boost::lexical_cast<boost::uuids::uuid>(projection->name())};

        for (size_t i = 0; i < synapses.size(); ++i)
        {
            synapses[i].params_.weight_ = weights[i];
            synapses[i].params_.delay_ = delays[i];
            synapses[i].params_.output_type_ = static_cast<synapse_traits::OutputType>(out_types[i]);
            synapses[i].id_from_ = source_ids[i];
            synapses[i].id_to_ = target_ids[i];
        }

        core::Projection<synapse_traits::DeltaSynapse> proj(
            uid_own, uid_from, uid_to,
            [&synapses](size_t i)
            { return std::make_tuple(synapses[i].params_, synapses[i].id_from_, synapses[i].id_to_); },
            synapses.size());
        result.push_back(std::move(proj));
    }
    return result;
}


std::string readFile(const std::string& path)
{
    namespace fs = std::filesystem;

    if (!fs::is_regular_file(path))
    {
        throw std::runtime_error("Path `" + path + "` is not a file");
    }

    std::ifstream file(path);

    if (file.fail())
    {
        throw std::runtime_error("Could not open file `" + path + "`");
    }

    std::string contents;

    file.seekg(0, std::ios::end);
    contents.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    contents.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return contents;
}


Network load_network(const fs::path& config_path)
{
    // Open and read config files
    fs::path nodes_path, edges_path;

    auto config = bbp::sonata::CircuitConfig::fromFile(config_path);
    auto list_edges = config.listEdgePopulations();
    auto list_nodes = config.listNodePopulations();
    Network network;

    for (auto& population_name : list_nodes)
    {
        // TODO : not just BLIFAT! Use type ids.
        auto snt_population = config.getNodePopulation(population_name);
        auto population = load_nodes<neuron_traits::BLIFATNeuron>(snt_population);
        network.add_population(std::move(population));
    }

    for (auto& projection_name : list_edges)
    {
        // TODO: Not just Delta. Use type ids.
        auto edges =
            load_edges<synapse_traits::DeltaSynapse>(config.getEdgePopulationProperties(projection_name).elementsPath);
        for (auto& proj : edges) network.add_projection(std::move(proj));
    }
    return network;
};

}  // namespace knp::framework
