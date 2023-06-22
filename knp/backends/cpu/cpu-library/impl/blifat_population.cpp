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


using knp::core::messaging::SynapticImpactMessage;

void impact_neuron(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron>::NeuronParameters &neuron,
    knp::synapse_traits::OutputType synapse_type, float impact_value)
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
    }
}


void calculate_neuron_state(knp::core::Population<knp::neuron_traits::BLIFATNeuron>::NeuronParameters &neuron)
{
    ++neuron.n_time_steps_since_last_firing_;
    neuron.dynamic_threshold_ *= neuron.threshold_decay_;
    neuron.postsynaptic_trace_ *= neuron.postsynaptic_trace_decay_;
    neuron.inhibitory_conductance_ *= neuron.inhibitory_conductance_decay_;

    if (neuron.bursting_phase_ && !--neuron.bursting_phase_)
    {
        neuron.potential_ = neuron.potential_ * neuron.potential_decay_ + neuron.reflexive_weight_;
    }
    else
    {
        neuron.potential_ *= neuron.potential_decay_;
    }
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

void calculate_neurons_state(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population,
    const std::vector<SynapticImpactMessage> &messages)
{
    SPDLOG_TRACE("Calculate neurons state");
    for (auto &neuron : population)
    {
        calculate_neuron_state(neuron);
    }

    process_inputs(population, messages);
}


bool calculate_neuron_post_input_state(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron>::NeuronParameters &neuron)
{
    bool spike = false;
    if (neuron.inhibitory_conductance_ < 1.0f)
    {
        neuron.potential_ -=
            (neuron.potential_ - neuron.reversive_inhibitory_potential_) * neuron.inhibitory_conductance_;
    }
    else
    {
        neuron.potential_ = neuron.reversive_inhibitory_potential_;
    }

    if ((neuron.n_time_steps_since_last_firing_ > neuron.absolute_refractory_period_) &&
        (neuron.potential_ >= 1.0 + neuron.dynamic_threshold_))
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


void calculate_neurons_post_input_state(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population,
    knp::core::messaging::SpikeData &neuron_indexes)
{
    SPDLOG_TRACE("Calculate neurons post input state");
    // can be made parallel
    for (size_t i = 0; i < population.size(); ++i)
    {
        if (calculate_neuron_post_input_state(population[i])) neuron_indexes.push_back(i);
    }
}


void calculate_blifat_population(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::MessageEndpoint &endpoint,
    size_t step_n)
{
    SPDLOG_DEBUG("Calculating BLIFAT population {}", std::string{population.get_uid()});
    // This whole function might be optimizable if we find a way to not loop over the whole population
    std::vector<SynapticImpactMessage> messages = endpoint.unload_messages<SynapticImpactMessage>(population.get_uid());

    calculate_neurons_state(population, messages);
    knp::core::messaging::SpikeData neuron_indexes;
    calculate_neurons_post_input_state(population, neuron_indexes);

    uint64_t time = step_n;
    if (!neuron_indexes.empty())
    {
        knp::core::messaging::SpikeMessage res_message{{population.get_uid(), time}, neuron_indexes};
        endpoint.send_message(res_message);
        SPDLOG_DEBUG("Sent {} spike(s)", res_message.neuron_indexes_.size());
    }
}
