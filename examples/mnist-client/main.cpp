/**
 * @file main.cpp
 * @brief An in-progress demo program for building and running MNIST-classification spiking network.
 * @author A. Vartenkov
 * @date 26.07.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/framework/monitoring/observer.h>
#include <knp/framework/network.h>
#include <knp/framework/sonata/network_io.h>

#include <iostream>
#include <limits>
#include <vector>

#include <boost/program_options.hpp>

#include "visualize_network.h"


using BLIFATParams = knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>;
using StdpBlifatNeuron = knp::neuron_traits::SynapticResourceSTDPBLIFATNeuron;
using StdpNeuronParams = knp::neuron_traits::neuron_parameters<StdpBlifatNeuron>;
using DeltaProjection = knp::core::Projection<knp::synapse_traits::DeltaSynapse>;
using BaseSynapse = DeltaProjection::Synapse;
using STDPDeltaProjection = knp::core::Projection<knp::synapse_traits::SynapticResourceSTDPDeltaSynapse>;
using STDPSynapse = STDPDeltaProjection::Synapse;

namespace po = boost::program_options;

const double activation_threshold = 8.531;


struct PopulationParams
{
    std::string name;
    int characteristic_time;
    double min_potential;
    std::vector<int> neurons;
    knp::core::UID uid;
    bool is_skipped = false;
    bool is_stdp = false;
};


struct ProjectionParams
{
    int ind_population_from;
    int ind_population_to;
    int connection_type;
    bool is_stdp = false;
};


std::vector<knp::synapse_traits::OutputType> ArNI_KNP_SynapseTypeTranslation{
    knp::synapse_traits::OutputType::EXCITATORY, knp::synapse_traits::OutputType::INHIBITORY_CURRENT,
    knp::synapse_traits::OutputType::DOPAMINE, knp::synapse_traits::OutputType::BLOCKING};


struct SynapseRule
{
    float min_weight_ = -0.698225F;
    float max_weight_ = 0.5F;
    float unconditional_resource_increase_ = 0.F;
    int last_step_ = -1000;
    size_t dopamine_plasticity_period_ = 5;
    float calculate_resource(float &weight) const
    {
        if (weight < min_weight_)
            weight = min_weight_;
        else if (weight >= max_weight_)
            weight = 0.96875F * max_weight_;  // max weight is unreachable and 1 - 1/32 is close enough.
        float resource = (weight - min_weight_) * (max_weight_ - min_weight_) / (max_weight_ - weight) * 1000.F;
        return resource;
    }

    [[nodiscard]] float calculate_resource(const float &weight) const
    {
        if (weight < min_weight_) return std::numeric_limits<float>::lowest();
        if (weight >= max_weight_) return std::numeric_limits<float>::max();
        return (weight - min_weight_) * (max_weight_ - min_weight_) / (max_weight_ - weight);
    }
} const default_synapse_rule;


bool can_be_stdp(const std::pair<const ProjectionParams, std::vector<int>> &params)
{
    static const int size_threshold = 100;
    return params.second.size() >= size_threshold;
}


struct STDPNeuronParamValues
{
    float hebbian_rule_coefficient_ = -0.13378F;
    size_t plasticity_time_ = 5;
    float starting_stability_ = 0;
    float synaptic_resource_threshold_ = 1;
    float stability_resource_change_ratio_ = 0.03635;
    float stability_change_at_isi_ = stability_resource_change_ratio_;
    float starting_synaptic_resource_ = 0;
    size_t n_silent_synapses_ = 44;
    uint64_t last_step_ = -1000;
} const default_stdp_neuron_params;


bool operator<(const ProjectionParams &a1, const ProjectionParams &a2)
{
    if (a1.ind_population_from != a2.ind_population_from) return a1.ind_population_from < a2.ind_population_from;
    if (a1.ind_population_to != a2.ind_population_to) return a1.ind_population_to < a2.ind_population_to;
    return a1.connection_type < a2.connection_type;
}


template <class T>
class Generator
{
public:
    Generator(const std::vector<int> &indices, const std::vector<T> &all) : indices_(indices), all_(all) {}
    T operator()(size_t index) { return all_[indices_[index]]; }

private:
    const std::vector<int> &indices_;
    const std::vector<T> &all_;
};


template <>
class Generator<STDPSynapse>
{
public:
    Generator(const std::vector<int> &indices, const std::vector<BaseSynapse> &all) : indices_(indices), all_(all) {}
    STDPSynapse operator()(size_t index)
    {
        BaseSynapse base_synapse = all_[indices_[index]];
        STDPSynapse result;
        std::get<knp::core::source_neuron_id>(result) = std::get<knp::core::source_neuron_id>(base_synapse);
        std::get<knp::core::target_neuron_id>(result) = std::get<knp::core::target_neuron_id>(base_synapse);
        auto &params = std::get<knp::core::synapse_data>(result);
        const auto &base_params = std::get<knp::core::synapse_data>(base_synapse);
        params.weight_ = base_params.weight_;
        params.output_type_ = base_params.output_type_;
        params.delay_ = base_params.delay_;
        params.rule_.w_min_ = default_synapse_rule.min_weight_;
        params.rule_.w_max_ = default_synapse_rule.max_weight_;
        params.rule_.d_u_ = default_synapse_rule.unconditional_resource_increase_;
        params.rule_.last_spike_step_ = default_synapse_rule.last_step_;
        params.rule_.dopamine_plasticity_period_ = default_synapse_rule.dopamine_plasticity_period_;
        params.rule_.synaptic_resource_ = default_synapse_rule.calculate_resource(params.weight_);
        return result;
    }

private:
    const std::vector<int> &indices_;
    const std::vector<BaseSynapse> &all_;
};


template <>
class Generator<StdpNeuronParams>
{
public:
    Generator(const std::vector<int> &indices, const std::vector<BLIFATParams> &all) : indices_(indices), all_(all) {}
    StdpNeuronParams operator()(size_t index)
    {
        StdpNeuronParams result(all_[indices_[index]]);
        result.stability_change_parameter_ = default_stdp_neuron_params.stability_resource_change_ratio_;
        result.isi_max_ = default_stdp_neuron_params.plasticity_time_;
        result.stability_ = default_stdp_neuron_params.starting_stability_;
        result.resource_drain_coefficient_ = default_stdp_neuron_params.n_silent_synapses_;
        result.synaptic_resource_threshold_ = default_stdp_neuron_params.synaptic_resource_threshold_;
        result.free_synaptic_resource_ = default_stdp_neuron_params.starting_synaptic_resource_;
        result.d_h_ = default_stdp_neuron_params.hebbian_rule_coefficient_;
        result.last_step_ = default_stdp_neuron_params.last_step_;
        result.stability_change_at_isi_ = default_stdp_neuron_params.stability_change_at_isi_;
        return result;
    }

private:
    const std::vector<int> &indices_;
    const std::vector<BLIFATParams> &all_;
};


knp::framework::Network create_network_from_monitoring_file(
    std::filesystem::path monitoring_file, int tact, const std::vector<std::string> &excluded_population_names,
    std::vector<knp::core::UID> &input_projection_uids, size_t output_population_index,
    knp::core::UID &output_population_uid, std::vector<std::string> stdp_population_names = {})
{
    knp::framework::Network network;
    std::ifstream moncsv(monitoring_file);
    int k;
    char comma;
    int neuron;

    if (!moncsv.good()) throw std::runtime_error(std::filesystem::absolute(monitoring_file).string() + " not found");

    std::string str;
    std::vector<PopulationParams> populations;
    std::map<ProjectionParams, std::vector<int>> projections;
    std::getline(moncsv, str);

    do
    {
        getline(moncsv, str);
        if (str.substr(0, 7) != "secsta,") break;
        if (str.back() == '\r') str.pop_back();
        std::stringstream ss(str.substr(7));
        PopulationParams pop;
        ss >> k >> comma >> pop.characteristic_time;
        if (pop.characteristic_time <= 0) throw std::runtime_error("Non-positive characteristic time encountered.");

        auto l = str.find_last_of(',');
        pop.name = str.substr(l + 1);

        if (std::find(excluded_population_names.begin(), excluded_population_names.end(), pop.name) !=
            excluded_population_names.end())
            pop.is_skipped = true;

        if (std::find(stdp_population_names.begin(), stdp_population_names.end(), pop.name) !=
            stdp_population_names.end())
            pop.is_stdp = true;
        else
            pop.is_stdp = false;

        l = str.find_last_of(',', l - 1);
        std::stringstream ss1(str.substr(l + 1));
        ss1 >> pop.min_potential;
        populations.push_back(pop);
    } while (true);

    if (populations.empty()) throw std::runtime_error(monitoring_file.string() + " has wrong format.");

    std::cout << "Populations count = " << populations.size() << std::endl;

    std::stringstream ss;
    ss << "neu->sec," << tact << ',';
    size_t n = ss.str().length();
    std::vector<std::pair<int, size_t>> neuron_populations;  // population id, neuron id

    while (!getline(moncsv, str).eof())
    {
        if (str.substr(0, n) == ss.str())
        {
            std::stringstream ss1(str.substr(n));
            int population;
            ss1 >> neuron >> comma >> population;

            if (populations[population].is_skipped) continue;

            if (neuron >= static_cast<int>(neuron_populations.size())) neuron_populations.resize(neuron + 1, {-1, 0});

            neuron_populations[neuron].first = population;
            neuron_populations[neuron].second = populations[population].neurons.size();
            populations[population].neurons.push_back(neuron);
        }
        else if (neuron_populations.size())
            break;
    }

    if (neuron_populations.empty())
        throw std::runtime_error(
            monitoring_file.string() + " has wrong format or does not contain tact #" + std::to_string(tact));

    // Now str contains description of the first neuron.

    std::stringstream ssneu;

    ssneu << "neu," << tact << ',';
    n = ssneu.str().length();
    std::stringstream sslin;
    sslin << "lin," << tact << ',';
    std::vector<BLIFATParams> all_neurons(neuron_populations.size());
    std::vector<BaseSynapse> all_synapses;

    // To a separate function
    do
    {
        if (str.substr(0, n) == ssneu.str())
        {
            std::stringstream neu_stream(str.substr(n));
            neu_stream >> neuron;
            if (neuron < static_cast<int>(neuron_populations.size()) && neuron_populations[neuron].first >= 0)
            {
                double real_number;
                neu_stream >> comma >> k >> comma >> k >> comma >> all_neurons[neuron].potential_ >> comma >>
                    real_number >> comma >> k >> comma >> k >> comma >> all_neurons[neuron].dynamic_threshold_ >>
                    comma >> all_neurons[neuron].threshold_decay_ >> comma >> all_neurons[neuron].threshold_increment_;
                all_neurons[neuron].potential_decay_ =
                    1 - 1. / populations[neuron_populations[neuron].first].characteristic_time;
                all_neurons[neuron].min_potential_ = populations[neuron_populations[neuron].first].min_potential;
                all_neurons[neuron].activation_threshold_ = activation_threshold;
            }
        }
        else if (str.substr(0, n) == sslin.str())
        {
            std::stringstream syn_stream(str.substr(n));
            BaseSynapse syn;
            syn_stream >> neuron;
            if (neuron < static_cast<int>(neuron_populations.size()) && neuron_populations[neuron].first >= 0)
            {
                ProjectionParams proj;
                int source;
                syn_stream >> comma >> proj.connection_type >> comma >> k >> comma >> k >> comma >>
                    std::get<knp::core::synapse_data>(syn).delay_ >> comma >> source;
                if ((source < 0 || (source - 1 < static_cast<int>(neuron_populations.size()) &&
                                    neuron_populations[source - 1].first >= 0)))
                {
                    syn_stream >> comma >> std::get<knp::core::synapse_data>(syn).weight_;
                    std::get<knp::core::target_neuron_id>(syn) = uint32_t(neuron_populations[neuron].second);
                    std::get<knp::core::source_neuron_id>(syn) =
                        source < 0 ? uint32_t(-1 - source) : uint32_t(neuron_populations[source - 1].second);
                    std::get<knp::core::synapse_data>(syn).output_type_ =
                        ArNI_KNP_SynapseTypeTranslation[proj.connection_type];

                    proj.ind_population_from = source < 0 ? -1 : neuron_populations[source - 1].first;
                    proj.ind_population_to = neuron_populations[neuron].first;

                    // If a projection leads to a STDP population and is not dopamine then it's considered STDP.
                    if (std::find(
                            stdp_population_names.begin(), stdp_population_names.end(),
                            populations[proj.ind_population_to].name) != stdp_population_names.end() &&
                        ArNI_KNP_SynapseTypeTranslation[proj.connection_type] !=
                            knp::synapse_traits::OutputType::DOPAMINE)
                        proj.is_stdp = true;  // This is an STDP projection.
                    else
                        proj.is_stdp = false;

                    projections[proj].push_back(all_synapses.size());
                    all_synapses.push_back(syn);
                }
            }
        }
        else
            break;
        str.clear();
        std::getline(moncsv, str);
    } while (true);

    // To a separate function
    for (size_t pop_index = 0; pop_index < populations.size(); ++pop_index)
    {
        auto &pop_params = populations[pop_index];
        if (pop_params.is_skipped) continue;
        if (pop_params.is_stdp)
        {
            knp::core::Population<knp::neuron_traits::SynapticResourceSTDPBLIFATNeuron> pop(
                Generator<StdpNeuronParams>(pop_params.neurons, all_neurons), pop_params.neurons.size());
            pop_params.uid = pop.get_uid();
            network.add_population(pop);
        }
        else
        {
            knp::core::Population<knp::neuron_traits::BLIFATNeuron> pop(
                Generator<BLIFATParams>(pop_params.neurons, all_neurons), pop_params.neurons.size());
            pop_params.uid = pop.get_uid();
            network.add_population(pop);
        }
        if (pop_index == output_population_index) output_population_uid = pop_params.uid;
    }

    // To a separate function
    for (const auto &projection_params : projections)
    {
        auto pop_from = projection_params.first.ind_population_from;
        auto pop_to = projection_params.first.ind_population_to;
        knp::core::UID projection_uid;
        if (projection_params.first.is_stdp && can_be_stdp(projection_params))
        {
            // This is an STDP projection, as a presynaptic to STDP populations
            STDPDeltaProjection pro(
                pop_from < 0 ? knp::core::UID{false} : populations[pop_from].uid, populations[pop_to].uid,
                Generator<STDPSynapse>(projection_params.second, all_synapses), projection_params.second.size());
            if (projection_params.first.ind_population_from < 0) input_projection_uids.push_back(pro.get_uid());
            pro.unlock_weights();
            // save_projection_to_file(pro, "projection_dump.txt", "img8_proj");
            network.add_projection(pro);
        }
        else
        {
            // This is a common delta projection
            DeltaProjection pro(
                pop_from < 0 ? knp::core::UID{false} : populations[pop_from].uid, populations[pop_to].uid,
                Generator<BaseSynapse>(projection_params.second, all_synapses), projection_params.second.size());
            if (projection_params.first.ind_population_from < 0) input_projection_uids.push_back(pro.get_uid());
            network.add_projection(pro);
        }
    }
    return network;
}


int main(int argc, char **argv)
{
    knp::core::UID output_uid;
    std::vector<knp::core::UID> input_uids;
    std::string task;
    std::string path_to_network;
    po::options_description options;
    options.add_options()("help,h", "Produce help message")(
        "task,t", po::value(&task), "Type of task: show, train, infer")(
        "net-path,p", po::value(&path_to_network), "File or directory for network storage.");
    po::variables_map options_map;
    po::store(po::parse_command_line(argc, argv, options), options_map);
    po::notify(options_map);

    if (!options_map.count("net-path")) return 0;

    knp::framework::Network network = create_network_from_monitoring_file(
        options_map["net-path"].as<std::string>(), 0, {}, input_uids, 0, output_uid);

    std::filesystem::create_directory("mnist_network");
    knp::framework::sonata::save_network(network, "mnist_network");
    std::cout << "Loaded" << std::endl;
    print_network_description(NetworkGraph(network));

    // Press ESC to exit or any key to iterate graph
    position_network_test(NetworkGraph(network), {0, 1, 2, 3, 4}, {1000, 700});
    // divide_graph_by_connectivity(NetworkGraph{network});
}
