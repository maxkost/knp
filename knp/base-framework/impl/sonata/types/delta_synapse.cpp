/**
 * @file delta_synapse.cpp
 * @brief Functions for loading and saving delta synapses.
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

#include <knp/core/projection.h>
#include <knp/synapse-traits/delta.h>

#include <spdlog/spdlog.h>

#include <filesystem>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>

#include "../csv_content.h"
#include "../highfive.h"
#include "../load_network.h"
#include "../save_network.h"
#include "type_id_defines.h"


namespace knp::framework::sonata
{

template <>
std::string get_synapse_type_name<synapse_traits::DeltaSynapse>()
{
    return "knp:DeltaSynapse";
}


template <>
core::Projection<knp::synapse_traits::DeltaSynapse> load_projection(
    const HighFive::Group &edges_group, const std::string &projection_name)
{
    SPDLOG_DEBUG("Loading edges for projection {}...", projection_name);
    auto projection_group = edges_group.getGroup(projection_name);
    auto group = projection_group.getGroup("0");
    size_t group_size = edges_group.getGroup(projection_name).getDataSet("edge_group_id").getDimensions().at(0);
    using SynapseParams = core::Projection<synapse_traits::DeltaSynapse>::SynapseParameters;
    using Synapse = core::Projection<synapse_traits::DeltaSynapse>::Synapse;

    std::vector<Synapse> target(group_size);
    const auto weights = read_parameter<decltype(SynapseParams::weight_)>(
        group, "syn_weight", group_size, synapse_traits::default_values<synapse_traits::DeltaSynapse>::weight_);
    const auto delays = read_parameter<decltype(SynapseParams::delay_)>(
        group, "delay", group_size, synapse_traits::default_values<synapse_traits::DeltaSynapse>::delay_);
    const auto out_types = read_parameter<int>(
        group, "output_type_", group_size,
        static_cast<size_t>(synapse_traits::default_values<synapse_traits::DeltaSynapse>::output_type_));
    const auto source_ids = read_parameter<size_t>(projection_group, "source_node_id", group_size, 0);
    const auto target_ids = read_parameter<size_t>(projection_group, "target_node_id", group_size, 0);

    const core::UID uid_from{boost::lexical_cast<boost::uuids::uuid>(
        projection_group.getDataSet("source_node_id").getAttribute("node_population").read<std::string>())};
    const core::UID uid_to{boost::lexical_cast<boost::uuids::uuid>(
        projection_group.getDataSet("target_node_id").getAttribute("node_population").read<std::string>())};
    const core::UID uid_own{boost::lexical_cast<boost::uuids::uuid>(projection_name)};


    std::vector<Synapse> synapses;
    synapses.reserve(group_size);
    for (size_t i = 0; i < weights.size(); ++i)
    {
        synapse_traits::synapse_parameters<synapse_traits::DeltaSynapse> syn;
        syn.weight_ = weights[i];
        syn.delay_ = delays[i];
        syn.output_type_ = static_cast<synapse_traits::OutputType>(out_types[i]);
        const size_t id_from = source_ids[i];
        const size_t id_to = target_ids[i];
        synapses.emplace_back(syn, id_from, id_to);
    }

    core::Projection<synapse_traits::DeltaSynapse> proj(
        uid_own, uid_from, uid_to, [&synapses](size_t syn_num) { return synapses[syn_num]; }, synapses.size());

    if (projection_group.hasAttribute("is_locked"))
    {
        if (projection_group.getAttribute("is_locked").read<bool>())
        {
            proj.lock_weights();
        }
        else
        {
            proj.unlock_weights();
        }
    }

    return proj;
}


template <>
void add_projection_to_h5<core::Projection<synapse_traits::DeltaSynapse>>(
    HighFive::File &file_h5, const knp::core::Projection<synapse_traits::DeltaSynapse> &projection)
{
    using SynapseParams = synapse_traits::synapse_parameters<synapse_traits::DeltaSynapse>;

    if (!file_h5.exist("edges")) throw std::runtime_error("File does not contain the \"edges\" group.");

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
        source_ids.push_back(std::get<knp::core::source_neuron_id>(v));
        target_ids.push_back(std::get<knp::core::target_neuron_id>(v));
        delays.push_back(std::get<knp::core::synapse_data>(v).delay_);
        weights.push_back(std::get<knp::core::synapse_data>(v).weight_);
        out_types.push_back(static_cast<int>(std::get<knp::core::synapse_data>(v).output_type_));
    }

    HighFive::Group proj_group = file_h5.createGroup("edges/" + std::string(projection.get_uid()));
    HighFive::DataSet source_node_dataset = proj_group.createDataSet("source_node_id", source_ids);
    source_node_dataset.createAttribute("node_population", std::string(projection.get_presynaptic()));

    HighFive::DataSet target_node_dataset = proj_group.createDataSet("target_node_id", target_ids);
    target_node_dataset.createAttribute("node_population", std::string(projection.get_postsynaptic()));

    // At the moment we support only one synapse group.
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
    proj_group.createAttribute("is_locked", projection.is_locked());
}

}  // namespace knp::framework::sonata
