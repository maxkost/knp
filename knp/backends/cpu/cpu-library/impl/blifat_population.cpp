/**
 * @file blifat_population.cpp
 * @brief BLIFAT neurons calculation routines implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/backends/cpu-library/blifat_population.h>
#include <knp/synapse-traits/output_types.h>

#include <spdlog/spdlog.h>

#include <string>
#include <unordered_map>


namespace knp::backends::cpu
{

using knp::core::messaging::SynapticImpactMessage;

void impact_neuron(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron>::NeuronParameters &neuron,
    const knp::synapse_traits::OutputType &synapse_type, float impact_value)
{
    switch (synapse_type)
    {
        case knp::synapse_traits::OutputType::EXCITATORY:
            neuron.potential_ += impact_value;
            break;
        case knp::synapse_traits::OutputType::INHIBITORY_CURRENT:
            neuron.potential_ -= impact_value;
            break;
        case knp::synapse_traits::OutputType::INHIBITORY_CONDUCTANCE:
            neuron.inhibitory_conductance_ += impact_value;
            break;
        case knp::synapse_traits::OutputType::DOPAMINE:
            break;
        case knp::synapse_traits::OutputType::BLOCKING:
            neuron.total_blocking_period_ = static_cast<unsigned int>(impact_value);
            break;
            //        default:
            //            const auto error_message = "Unknown synapse type: " +
            //            std::to_string(static_cast<int>(synapse_type)); SPDLOG_ERROR(error_message); throw
            //            std::runtime_error(error_message);
    }
}


void calculate_neuron_state(knp::core::Population<knp::neuron_traits::BLIFATNeuron>::NeuronParameters &neuron)
{
    neuron.dynamic_threshold_ *= neuron.threshold_decay_;
    neuron.postsynaptic_trace_ *= neuron.postsynaptic_trace_decay_;
    neuron.inhibitory_conductance_ *= neuron.inhibitory_conductance_decay_;

    if (neuron.bursting_phase_ && !--neuron.bursting_phase_)
        neuron.potential_ = neuron.potential_ * neuron.potential_decay_ + neuron.reflexive_weight_;
    else
        neuron.potential_ *= neuron.potential_decay_;
}


void process_inputs(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population,
    const std::vector<SynapticImpactMessage> &messages)
{
    SPDLOG_TRACE("Process inputs");
    for (const auto &message : messages)
    {
        for (const auto &impact : message.impacts_)
        {
            impact_neuron(population[impact.postsynaptic_neuron_index_], impact.synapse_type_, impact.impact_value_);
        }
    }
}


void calculate_neurons_state_part(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, size_t part_start, size_t part_size)
{
    size_t part_end = std::min(part_start + part_size, population.size());
    SPDLOG_TRACE("Calculate neurons state part");
    for (size_t i = part_start; i < part_end; ++i)
    {
        auto &neuron = population[i];
        ++neuron.n_time_steps_since_last_firing_;
        if (neuron.total_blocking_period_)
        {
            --neuron.total_blocking_period_;
        }
        else
        {
            calculate_neuron_state(neuron);
        }
    }
}


void calculate_neurons_state(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population,
    const std::vector<SynapticImpactMessage> &messages)
{
    calculate_neurons_state_part(population, 0, population.size());
    process_inputs(population, messages);
}


bool calculate_neuron_post_input_state(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron>::NeuronParameters &neuron)
{
    bool spike = false;

    if (neuron.inhibitory_conductance_ < 1.0)
    {
        neuron.potential_ -=
            (neuron.potential_ - neuron.reversive_inhibitory_potential_) * neuron.inhibitory_conductance_;
    }
    else
    {
        neuron.potential_ = neuron.reversive_inhibitory_potential_;
    }

    if ((neuron.n_time_steps_since_last_firing_ > neuron.absolute_refractory_period_) &&
        (neuron.potential_ >= neuron.activation_threshold_ + neuron.dynamic_threshold_))
    {
        // Spike.
        neuron.dynamic_threshold_ += neuron.threshold_increment_;
        neuron.postsynaptic_trace_ += neuron.postsynaptic_trace_increment_;

        neuron.potential_ = neuron.potential_reset_value_;
        neuron.bursting_phase_ = neuron.bursting_period_;
        neuron.n_time_steps_since_last_firing_ = 0;
        spike = true;
    }

    if (neuron.potential_ < neuron.min_potential_) neuron.potential_ = neuron.min_potential_;

    return spike;
}


void calculate_neurons_post_input_state_part(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::messaging::SpikeMessage &message,
    size_t part_start, size_t part_size, std::mutex &mutex)
{
    SPDLOG_TRACE("Calculate neurons post input state part");
    size_t part_end = std::min(part_start + part_size, population.size());
    std::vector<size_t> output;
    for (size_t i = part_start; i < part_end; ++i)
        if (!population[i].total_blocking_period_ && calculate_neuron_post_input_state(population[i]))
            output.push_back(i);

    // Updating common neuron indexes.
    std::lock_guard<std::mutex> lock(mutex);
    message.neuron_indexes_.reserve(message.neuron_indexes_.size() + output.size());
    message.neuron_indexes_.insert(message.neuron_indexes_.end(), output.begin(), output.end());
}


void calculate_neurons_post_input_state(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population,
    knp::core::messaging::SpikeData &neuron_indexes)
{
    SPDLOG_TRACE("Calculate neurons post input state part");
    for (size_t index = 0; index < population.size(); ++index)
    {
        if (!population[index].total_blocking_period_ && calculate_neuron_post_input_state(population[index]))
            neuron_indexes.push_back(index);
    }
}


knp::core::messaging::SpikeData calculate_blifat_population_data(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::MessageEndpoint &endpoint)
{
    SPDLOG_DEBUG("Calculating BLIFAT population {}", std::string{population.get_uid()});
    // This whole function might be optimizable if we find a way to not loop over the whole population.
    std::vector<SynapticImpactMessage> messages = endpoint.unload_messages<SynapticImpactMessage>(population.get_uid());

    calculate_neurons_state(population, messages);
    knp::core::messaging::SpikeData neuron_indexes;
    calculate_neurons_post_input_state(population, neuron_indexes);

    return neuron_indexes;
}


void calculate_blifat_population(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::MessageEndpoint &endpoint,
    size_t step_n)
{
    auto neuron_indexes{calculate_blifat_population_data(population, endpoint)};

    if (!neuron_indexes.empty())
    {
        knp::core::messaging::SpikeMessage res_message{{population.get_uid(), step_n}, neuron_indexes};
        endpoint.send_message(res_message);
        SPDLOG_DEBUG("Sent {} spike(s)", res_message.neuron_indexes_.size());
    }
}


void calculate_blifat_population(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::MessageEndpoint &endpoint,
    size_t step_n, std::mutex &mutex)
{
    auto neuron_indexes{calculate_blifat_population_data(population, endpoint)};

    if (!neuron_indexes.empty())
    {
        knp::core::messaging::SpikeMessage res_message{{population.get_uid(), step_n}, neuron_indexes};
        std::lock_guard<std::mutex> guard(mutex);
        endpoint.send_message(res_message);
        SPDLOG_DEBUG("Sent {} spike(s)", res_message.neuron_indexes_.size());
    }
}

}  // namespace knp::backends::cpu
