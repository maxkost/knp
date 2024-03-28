//
// Created by an_vartenkov on 28.03.24.
//
#include <knp/core/population.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/neuron-traits/stdp_synaptic_resource_rule.h>

#include <spdlog/spdlog.h>

#include <highfive/highfive.hpp>

#include "save_load_network.h"
#include "type_id_defines.h"


namespace knp::framework
{
template <>
int get_neuron_type_id<neuron_traits::SynapticResourceSTDPBLIFATNeuron>()
{
    return 1100;
}


template <>
std::string get_neuron_type_name<neuron_traits::SynapticResourceSTDPBLIFATNeuron>()
{
    return "knp:SynapticResourceRuleBlifatNeuron";
}


template <>
void add_population_to_h5<core::Population<knp::neuron_traits::SynapticResourceSTDPBLIFATNeuron>>(
    HighFive::File &file_h5, const core::Population<knp::neuron_traits::SynapticResourceSTDPBLIFATNeuron> &population)
{
    SPDLOG_TRACE("Adding population to hdf5");

    if (!file_h5.exist("nodes")) throw std::runtime_error("File doesn't contain \"nodes\" group");

    HighFive::Group population_group = file_h5.createGroup("nodes/" + std::string(population.get_uid()));

    std::vector<int> type_ids;
    std::vector<size_t> neuron_ids;
    std::vector<int> neuron_type_ids(
        population.size(), get_neuron_type_id<neuron_traits::SynapticResourceSTDPBLIFATNeuron>());

    neuron_ids.reserve(population.size());

    for (size_t i = 0; i < population.size(); ++i) neuron_ids.push_back(i);

    population_group.createDataSet("node_id", neuron_ids);
    population_group.createDataSet("node_group_index", neuron_ids);
    population_group.createDataSet("node_group_id", std::vector<size_t>(population.size(), 0));
    population_group.createDataSet(
        "node_type_id",
        std::vector<size_t>(population.size(), get_neuron_type_id<neuron_traits::SynapticResourceSTDPBLIFATNeuron>()));
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

}  // namespace knp::framework
