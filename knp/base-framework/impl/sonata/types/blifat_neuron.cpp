/**
 * @file save_load_blifat.cpp
 * @brief BLIFAT neuron procedures file.
 * @author An. Vartenkov
 * @date 28.03.2024
 */

#include <knp/core/population.h>
#include <knp/neuron-traits/blifat.h>

#include <spdlog/spdlog.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>

#include <highfive/highfive.hpp>

#include "../csv_content.h"
#include "../save_load_network.h"
#include "type_id_defines.h"


namespace knp::framework
{

template <>
int get_neuron_type_id<neuron_traits::BLIFATNeuron>()
{
    return 1000;
}


template <>
std::string get_neuron_type_name<neuron_traits::BLIFATNeuron>()
{
    return "knp:BasicBlifatNeuron";
}


template <>
void add_population_to_h5<core::Population<knp::neuron_traits::BLIFATNeuron>>(
    HighFive::File &file_h5, const core::Population<knp::neuron_traits::BLIFATNeuron> &population)
{
    SPDLOG_TRACE("Adding population to hdf5");

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
    PUT_NEURON_TO_DATASET(population, n_time_steps_since_last_firing_, group0);
    PUT_NEURON_TO_DATASET(population, activation_threshold_, group0);
    PUT_NEURON_TO_DATASET(population, threshold_decay_, group0);
    PUT_NEURON_TO_DATASET(population, threshold_increment_, group0);
    PUT_NEURON_TO_DATASET(population, postsynaptic_trace_, group0);
    PUT_NEURON_TO_DATASET(population, postsynaptic_trace_decay_, group0);
    PUT_NEURON_TO_DATASET(population, postsynaptic_trace_increment_, group0);
    PUT_NEURON_TO_DATASET(population, inhibitory_conductance_, group0);
    PUT_NEURON_TO_DATASET(population, inhibitory_conductance_decay_, group0);
    PUT_NEURON_TO_DATASET(population, potential_decay_, group0);
    PUT_NEURON_TO_DATASET(population, bursting_period_, group0);
    PUT_NEURON_TO_DATASET(population, reflexive_weight_, group0);
    PUT_NEURON_TO_DATASET(population, reversal_inhibitory_potential_, group0);
    PUT_NEURON_TO_DATASET(population, absolute_refractory_period_, group0);
    PUT_NEURON_TO_DATASET(population, potential_reset_value_, group0);
    PUT_NEURON_TO_DATASET(population, min_potential_, group0);

    auto dynamic_group0 = group0.createGroup("dynamics_params");
    // Dynamic
    PUT_NEURON_TO_DATASET(population, dynamic_threshold_, dynamic_group0);
    PUT_NEURON_TO_DATASET(population, potential_, dynamic_group0);
    PUT_NEURON_TO_DATASET(population, pre_impact_potential_, dynamic_group0);
    PUT_NEURON_TO_DATASET(population, bursting_phase_, dynamic_group0);
    PUT_NEURON_TO_DATASET(population, total_blocking_period_, dynamic_group0);
    PUT_NEURON_TO_DATASET(population, dopamine_value_, dynamic_group0);
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
core::Population<neuron_traits::BLIFATNeuron> load_population<neuron_traits::BLIFATNeuron>(
    const HighFive::Group &nodes_group, const std::string &population_name)
{
    SPDLOG_DEBUG("Loading nodes");
    auto group = nodes_group.getGroup(population_name).getGroup("0");
    size_t group_size = nodes_group.getGroup(population_name).getDataSet("node_id").getDimensions().at(0);

    std::vector<core::Population<neuron_traits::BLIFATNeuron>> result;
    // TODO: Load default neuron from json file.
    std::vector<neuron_traits::neuron_parameters<neuron_traits::BLIFATNeuron>> target(group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, n_time_steps_since_last_firing_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, activation_threshold_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, threshold_decay_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, threshold_increment_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, postsynaptic_trace_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, postsynaptic_trace_decay_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, postsynaptic_trace_increment_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, inhibitory_conductance_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, inhibitory_conductance_decay_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, potential_decay_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, bursting_period_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, reflexive_weight_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, reversal_inhibitory_potential_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, absolute_refractory_period_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, potential_reset_value_, group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, min_potential_, group, group_size);

    // Dynamic
    auto dyn_group = group.getGroup("dynamics_params");
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, dynamic_threshold_, dyn_group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, potential_, dyn_group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, pre_impact_potential_, dyn_group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, bursting_phase_, dyn_group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, total_blocking_period_, dyn_group, group_size);
    LOAD_NEURONS_PARAMETER(target, neuron_traits::BLIFATNeuron, dopamine_value_, dyn_group, group_size);

    knp::core::UID uid{boost::lexical_cast<boost::uuids::uuid>(population_name)};
    core::Population<neuron_traits::BLIFATNeuron> out_population(
        uid, [&target](size_t i) { return target[i]; }, group_size);
    return out_population;
}


}  // namespace knp::framework
