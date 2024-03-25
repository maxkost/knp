//
// Created by an_vartenkov on 31.01.24.
//

#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/framework/network.h>

#include <bbp/sonata/common.h>
#include <spdlog/spdlog.h>

#include <filesystem>

#include <boost/format.hpp>

#include <highfive/H5File.hpp>

#include "csv_content.h"
#include "type_id_defines.h"

namespace knp::framework
{
namespace fs = std::filesystem;


template <class Projection>
void add_projection_to_h5(HighFive::File &, const Projection &)
{
    throw std::runtime_error("Couldn't save projection of unknown class");
}


template <class Population>
void add_population_to_h5(HighFive::File &, const Population &)
{
    throw std::runtime_error("Saving model: unknown population type");
}


// Can probably use template instead, but it would be just a bit more complex and repetitive.
#define PUT_NEURON_TO_DATASET(pop, param, group)                                                                \
    {                                                                                                           \
        std::vector<decltype(pop.begin()->param)> data;                                                         \
        data.reserve(pop.size());                                                                               \
        std::transform(                                                                                         \
            pop.begin(), pop.end(), std::back_inserter(data), [](const auto &neuron) { return neuron.param; }); \
        group.createDataSet(#param, data);                                                                      \
    }


template <>
void add_population_to_h5<core::Population<knp::neuron_traits::BLIFATNeuron>>(
    HighFive::File &file_h5, const core::Population<knp::neuron_traits::BLIFATNeuron> &population)
{
    if (!file_h5.exist("nodes")) throw std::runtime_error("File doesn't contain \"nodes\" group");

    HighFive::Group population_group = file_h5.createGroup("nodes/" + std::string(population.get_uid()));

    std::vector<int> type_ids;
    std::vector<size_t> neuron_ids;
    std::vector<int> neuron_type_ids(population.size(), get_neuron_type_id<neuron_traits::BLIFATNeuron>());
    neuron_ids.reserve(population.size());
    for (size_t i = 0; i < population.size(); ++i) neuron_ids.push_back(i);

    population_group.createDataSet("node_id", neuron_ids);
    population_group.createDataSet("node_group_index", neuron_ids);
    population_group.createDataSet("node_group_id", std::vector<size_t>(population.size(), 0));
    population_group.createDataSet(
        "node_type_id", std::vector<size_t>(population.size(), get_neuron_type_id<neuron_traits::BLIFATNeuron>()));
    auto group0 = population_group.createGroup("0");

    // TODO: This function need to check if all parameters are the same, then not save those into h5.
    // Static
    PUT_NEURON_TO_DATASET(population, n_time_steps_since_last_firing_, group0)
    PUT_NEURON_TO_DATASET(population, activation_threshold_, group0)
    PUT_NEURON_TO_DATASET(population, threshold_decay_, group0)
    PUT_NEURON_TO_DATASET(population, threshold_increment_, group0)
    PUT_NEURON_TO_DATASET(population, postsynaptic_trace_, group0)
    PUT_NEURON_TO_DATASET(population, postsynaptic_trace_decay_, group0)
    PUT_NEURON_TO_DATASET(population, postsynaptic_trace_increment_, group0)
    PUT_NEURON_TO_DATASET(population, inhibitory_conductance_, group0)
    PUT_NEURON_TO_DATASET(population, inhibitory_conductance_decay_, group0)
    PUT_NEURON_TO_DATASET(population, potential_decay_, group0)
    PUT_NEURON_TO_DATASET(population, bursting_period_, group0)
    PUT_NEURON_TO_DATASET(population, reflexive_weight_, group0)
    PUT_NEURON_TO_DATASET(population, reversive_inhibitory_potential_, group0)
    PUT_NEURON_TO_DATASET(population, absolute_refractory_period_, group0)
    PUT_NEURON_TO_DATASET(population, potential_reset_value_, group0)
    PUT_NEURON_TO_DATASET(population, min_potential_, group0)

    auto dynamic_group0 = group0.createGroup("dynamics_params");
    // Dynamic
    PUT_NEURON_TO_DATASET(population, dynamic_threshold_, dynamic_group0)
    PUT_NEURON_TO_DATASET(population, potential_, dynamic_group0)
    PUT_NEURON_TO_DATASET(population, pre_impact_potential_, dynamic_group0)
    PUT_NEURON_TO_DATASET(population, bursting_phase_, dynamic_group0)
    PUT_NEURON_TO_DATASET(population, total_blocking_period_, dynamic_group0)
    PUT_NEURON_TO_DATASET(population, dopamine_value_, dynamic_group0)
}


template <>
void add_projection_to_h5<core::Projection<synapse_traits::DeltaSynapse>>(
    HighFive::File &file_h5, const knp::core::Projection<synapse_traits::DeltaSynapse> &projection)
{
    if (!file_h5.exist("edges")) throw std::runtime_error("File doesn't contain \"edges\" group.");

    std::vector<uint64_t> source_ids, target_ids;
    std::vector<uint64_t> delays;
    std::vector<double> weights;
    std::vector<uint32_t> out_types;
    source_ids.reserve(projection.size());
    target_ids.reserve(projection.size());
    delays.reserve(projection.size());
    weights.reserve(projection.size());
    out_types.reserve(projection.size());

    for (const auto &v : projection)
    {
        source_ids.push_back(v.id_from_);
        target_ids.push_back(v.id_to_);
        delays.push_back(v.params_.delay_);
        weights.push_back(v.params_.weight_);
        out_types.push_back(static_cast<uint32_t>(v.params_.output_type_));
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


template <class Synapse>
void add_synapse_type_to_csv(const fs::path &)
{
    throw std::logic_error("Unknown synapse type");
}


const std::vector<std::string> edge_file_header = {"edge_type_id", "dynamics_params", "model_template"};
const std::vector<std::string> node_file_header = {"node_type_id", "model_type", "model_template", "model_name"};


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


template <class Neuron>
void add_neuron_type_to_csv(const fs::path &)
{
    throw std::logic_error("Unknown neuron type");
}


template <>
void add_neuron_type_to_csv<neuron_traits::BLIFATNeuron>(const fs::path &csv_path)
{
    CsvContent csv_file;
    if (fs::is_regular_file(csv_path))
    {
        // File already exists, load it.
        csv_file.load(csv_path);
        // Check header correctness.
        auto file_header = csv_file.get_header();
        for (const auto &column_name : node_file_header)
        {
            if (std::find(file_header.begin(), file_header.end(), column_name) == file_header.end())
                throw std::runtime_error("Couldn't find column: " + column_name + " in file " + csv_path.string());
        }
        // Header is okay, check if type exists already.
        size_t height = csv_file.get_rc_size().first - 1;
        for (size_t row_id = 0; row_id < height; ++row_id)
        {
            if (csv_file.get_value<int>(row_id, "node_type_id") == get_neuron_type_id<neuron_traits::BLIFATNeuron>())
            {
                return;  // Type exists, nothing to add.
            }
        }
    }
    else
    {
        csv_file.set_header(node_file_header);
    }
    // Add type
    std::vector<std::string> type_row = {
        std::to_string(get_neuron_type_id<neuron_traits::BLIFATNeuron>()), "point_neuron", "",
        get_neuron_type_name<neuron_traits::BLIFATNeuron>()};
    csv_file.add_row(type_row);
    csv_file.save(csv_path);
}


template <>
void add_neuron_type_to_csv<VirtualNeuron>(const fs::path &csv_path)
{
    // Add virtual neurons
    CsvContent csv_file;
    if (fs::is_regular_file(csv_path))
    {
        // File already exists, load it.
        csv_file.load(csv_path);
        // Check header correctness.
        auto file_header = csv_file.get_header();
        for (const auto &column_name : node_file_header)
        {
            if (std::find(file_header.begin(), file_header.end(), column_name) == file_header.end())
                throw std::runtime_error("Couldn't find column: " + column_name + " in file " + csv_path.string());
        }
        // Header is okay, check if type exists already.
        size_t height = csv_file.get_rc_size().first - 1;
        for (size_t row_id = 0; row_id < height; ++row_id)
        {
            if (csv_file.get_value<int>(row_id, "node_type_id") == get_neuron_type_id<VirtualNeuron>())
            {
                return;  // Type exists, nothing to add.
            }
        }
    }
    else
    {
        csv_file.set_header(node_file_header);
    }
    // Add type
    std::vector<std::string> type_row = {
        std::to_string(get_neuron_type_id<neuron_traits::BLIFATNeuron>()), "virtual", "",
        get_neuron_type_name<neuron_traits::BLIFATNeuron>()};
    csv_file.add_row(type_row);
    csv_file.save(csv_path);
}


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
    }

    // TODO : move this inside add_projection
    add_synapse_type_to_csv<synapse_traits::DeltaSynapse>(path_to_synapse_csv);

    fs::path path_to_populations(net_dir / populations_filename);
    fs::path path_to_neurons_csv(net_dir / neuron_type_filename);
    HighFive::File h5_pop_file(path_to_populations.string(), HighFive::File::Create | HighFive::File::Overwrite);
    h5_pop_file.createGroup("nodes");
    for (auto iter = network.begin_populations(); iter != network.end_populations(); ++iter)
    {
        std::visit([&h5_pop_file](const auto &population) { add_population_to_h5(h5_pop_file, population); }, *iter);
    }
    // TODO : move this inside add_population
    add_neuron_type_to_csv<neuron_traits::BLIFATNeuron>(path_to_neurons_csv);

    write_base_config(dir, net_dir);
    write_network_config(
        net_dir / "network_config.json", populations_filename, projections_filename, neuron_type_filename,
        synapse_type_filename, network);
}

}  // namespace knp::framework
