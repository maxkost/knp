/**
 * @file synaptic_resource_stdp.cpp
 * @brief STDP calculation routines implementation.
 * @author Artiom N.
 * @date 21.08.2023
 */
#include "knp/backends/cpu-library/synaptic_resource_stdp.h"

#include <knp/backends/cpu-library/blifat_population.h>
#include <knp/backends/cpu-library/delta_synapse_projection.h>

#include <spdlog/spdlog.h>

#include <unordered_map>
#include <vector>


namespace knp::backends::cpu
{
using knp::core::messaging::SpikeMessage;

template <class Synapse>
using STDPSynapseParams = knp::synapse_traits::synapse_parameters<
    knp::synapse_traits::STDP<knp::synapse_traits::STDPSynapticResourceRule, synapse_traits::DeltaSynapse>>;


/**
 * @brief Recalculate synapse weights from synaptic resource.
 * @tparam WeightedSynapse synapse that has "weight_" parameter.
 * @param synapse_params synapse parameters.
 */
template <class WeightedSynapse>
void recalculate_synapse_weights(std::vector<STDPSynapseParams<WeightedSynapse> *> &synapse_params)
{
    // Synapse weight recalculation.
    for (auto synapse_ptr : synapse_params)
    {
        const auto syn_w = std::max(synapse_ptr->rule_.synaptic_resource_, 0.f);
        const auto weight_diff = synapse_ptr->rule_.w_max_ - synapse_ptr->rule_.w_min_;
        synapse_ptr->weight_ = synapse_ptr->rule_.w_min_ + weight_diff * syn_w / (weight_diff + syn_w);
    }
}

template <class Synapse>
using StdpProjection =
    knp::core::Projection<knp::synapse_traits::STDP<knp::synapse_traits::STDPSynapticResourceRule, Synapse>>;

/**
 * @brief Apply STDP to all presynaptic connections of a single population.
 * @tparam NeuronType A type of neuron that is compatible with STDP.
 * @param msg spikes emited by population.
 * @param projections All projections. The ones that are not connected, are locked or are of a wrong type are skipped.
 * @param population population.
 * @param step current network step.
 * @param post_synaptic
 * @note all projections are supposed to be of the same type.
 */
template <class NeuronType>
void process_spiking_neurons(
    const SpikeMessage &msg, std::vector<StdpProjection<synapse_traits::DeltaSynapse> *> &working_projections,
    knp::core::Population<knp::neuron_traits::SynapticResourceSTDPNeuron<NeuronType>> &population, uint64_t step)
{
    // It's very important that during this function no projection invalidates iterators.
    using SynapseType =
        knp::synapse_traits::STDP<knp::synapse_traits::STDPSynapticResourceRule, synapse_traits::DeltaSynapse>;
    using SynapseParamType = knp::synapse_traits::synapse_parameters<SynapseType>;

    // Loop over neurons
    for (const auto &spiked_neuron_index : msg.neuron_indexes_)
    {
        std::vector<SynapseParamType *> synapse_params;
        for (auto *projection : working_projections)
            for (auto synapse_index : projection->get_by_postsynaptic_neuron(spiked_neuron_index))
                synapse_params.push_back(&(*projection)[synapse_index].params_);

        auto &neuron = population[spiked_neuron_index];
        // Calculate neuron ISI status.
        neuron_traits::update_isi<neuron_traits::BLIFATNeuron>(neuron, step);
        // Update synapse-only data
        for (auto *synapse : synapse_params)
        {
            // Unconditional decreasing synaptic resource.
            synapse->rule_.synaptic_resource_ -= synapse->rule_.d_u_;
            neuron.free_synaptic_resource_ += synapse->rule_.d_u_;
            // Hebbian plasticity.
            // 1. check if synapse ever got a spike in the current ISI period
            if (step - synapse->rule_.last_spike_step_ >=
                neuron.first_isi_spike_ - synapse->rule_.dopamine_plasticity_period_)
            {
                // 2. If it did then update synaptic resource value.
                const float d_h = neuron.d_h_ * std::min(static_cast<float>(std::pow(2, -neuron.stability_)), 1.f);
                synapse->rule_.synaptic_resource_ += d_h;
                neuron.free_synaptic_resource_ -= d_h;
            }
        }

        // Free synaptic resource renormalization.
        // TODO : This should be in a different function.
        if (neuron_traits::ISIPeriodType::period_started == neuron.isi_status_ &&
            abs(neuron.free_synaptic_resource_) > neuron.free_synaptic_resource_threshold_)
        {
            auto add_resource_value =
                neuron.free_synaptic_resource_ / (synapse_params.size() + neuron.resource_drain_coefficient_);

            for (auto synapse : synapse_params) synapse->rule_.synaptic_resource_ += add_resource_value;

            neuron.free_synaptic_resource_ = 0.0F;
        }

        // Dopamine processing. Dopamine punishment if forced does nothing.
        if (neuron.dopamine_value_ > 0.0 ||
            (neuron.dopamine_value_ < 0.0 && neuron.isi_status != neuron_traits::ISIPeriodType::is_forced))
        {
            // Change synapse values for both D > 0 and D < 0
            for (auto synapse : synapse_params)
            {
                if (step - synapse->rule_.last_spike_step_ < synapse->rule_.dopamine_plasticity_period_)
                    // Change synapse resource
                    synapse->rule_.synaptic_resource_ +=
                        neuron.dopamine_value * std::min(static_cast<float>(std::pow(2, -neuron.stability_)), 1.F);
            }
            // Stability changes
            if (neuron.isi_status == neuron_traits::ISIPeriodType::is_forced || neuron.dopamine_value_ < 0)
                // A dopamine reward when forced or a dopamine punishment reduce stability by r*D
                neuron.stability_ -= neuron.dopamine_value_ * neuron.stability_change_parameter_;
            else
            {
                // A dopamine reward when non-forced changes stability by D max(2 - |t(TSS) - ISImax| / ISImax, -1)
                const double dopamine_constant = 2.0;
                neuron.stability_ +=
                    neuron.dopamine_value_ *
                    std::max(
                        dopamine_constant - abs(step - neuron.first_isi_spike_ - neuron.isi_max_) / neuron.isi_max_,
                        -1);
            }
        }
        if (neuron.isi_status == neuron_traits::ISIPeriodType::period_started) neuron.last_unforced_spike_ = step;

        // Recalculating synapse weights. Sometimes it probably doesn't need to happen, check it later.
        recalculate_synapse_weights<knp::synapse_traits::DeltaSynapse>(synapse_params);
    }
}


}  // namespace knp::backends::cpu
