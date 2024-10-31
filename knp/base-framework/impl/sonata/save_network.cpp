/**
 * @file save_network.cpp
 * @brief Functions for saving and loading network.
 * @kaspersky_support A. Vartenkov
 * @date 09.04.2024
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

#include "save_network.h"

#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/framework/network.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <spdlog/spdlog.h>

#include <filesystem>

#include <boost/format.hpp>

#include "csv_content.h"
#include "highfive.h"
#include "types/type_id_defines.h"


namespace knp::framework::sonata
{
namespace fs = std::filesystem;


void write_base_config(const fs::path &config_dir, const fs::path &net_config_dir)
{
    auto net_config_path = net_config_dir / "network_config.json";
    std::string base_config = R"({"networks": ["%s"]})";
    boost::format base_config_formatter(base_config);
    base_config = (base_config_formatter % net_config_path.string()).str();
    std::ofstream base_config_file(config_dir / "config.json");
    base_config_file.exceptions(std::ofstream::badbit | std::ofstream::failbit);
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
    result.resize(result.size() - 1);  // Remove last comma.
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
    result.resize(result.size() - 1);  // Remove last comma.
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
    file_out.exceptions(std::ofstream::badbit | std::ofstream::failbit);
    file_out << res_string;
}


KNP_DECLSPEC void save_network(const Network &network, const fs::path &dir)
{
    auto net_dir = dir / "network";
    if (!is_directory(net_dir)) fs::create_directory(net_dir);

    const std::string projections_filename = "projections.h5";
    const std::string synapse_type_filename = "synapses.csv";
    const std::string populations_filename = "populations.h5";
    const std::string neuron_type_filename = "neurons.csv";

    fs::path path_to_projections(net_dir / projections_filename);
    fs::path path_to_synapse_csv(net_dir / synapse_type_filename);

    HighFive::File h5_proj_file(path_to_projections.string(), HighFive::File::Create | HighFive::File::Overwrite);
    h5_proj_file.createGroup("edges");

    for (auto iter = network.begin_projections(); iter != network.end_projections(); ++iter)
    {
        std::visit([&h5_proj_file](const auto &projection) { add_projection_to_h5(h5_proj_file, projection); }, *iter);
        std::visit(
            [&path_to_synapse_csv](const auto &projection) {
                add_synapse_type_to_csv<typename std::decay_t<decltype(projection)>::ProjectionSynapseType>(
                    path_to_synapse_csv);
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
    // TODO: Move this inside add_population or add more neurons.
    add_neuron_type_to_csv<neuron_traits::BLIFATNeuron>(path_to_neurons_csv);

    write_base_config(dir, net_dir);
    write_network_config(
        net_dir / "network_config.json", populations_filename, projections_filename, neuron_type_filename,
        synapse_type_filename, network);
}

}  // namespace knp::framework::sonata
