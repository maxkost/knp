/**
 * @file synaptic_resource_stdp.h
 * @brief STDP calculation routines implementation.
 * @author Vartenkov A.
 * @date 12.10.2023
 */

#pragma once
#include <knp/core/messaging/spike_message.h>
#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/neuron-traits/synaptic_resource_stdp.h>
#include <knp/synapse-traits/stdp_common.h>
#include <knp/synapse-traits/stdp_synaptic_resource_rule.h>
#include <knp/synapse-traits/stdp_type_traits.h>

#include <algorithm>
#include <vector>

namespace knp::backends::cpu
{
template <class Synapse>
using STDPSynapseParams = knp::synapse_traits::synapse_parameters<
    knp::synapse_traits::STDP<knp::synapse_traits::STDPSynapticResourceRule, Synapse>>;


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

bool is_point_in_interval(uint64_t interval_begin, uint64_t interval_end, uint64_t point)
{
    const bool is_after_begin = point >= interval_begin;
    const bool is_before_end = point <= interval_end;
    const bool is_overflow = interval_end < interval_begin;
    return (is_after_begin && is_before_end) || ((is_after_begin || is_before_end) && is_overflow);
}

template <class SynapseType>
std::vector<synapse_traits::synapse_parameters<SynapseType> *> get_all_connected_synapses(
    const std::vector<core::Projection<SynapseType> *> &projections_to_neuron, size_t neuron_index)
{
    std::vector<synapse_traits::synapse_parameters<SynapseType> *> result;
    for (auto *projection : projections_to_neuron)
    {
        auto synapses = projection->get_by_postsynaptic_neuron(neuron_index);
        std::transform(
            synapses.begin(), synapses.end(), std::back_inserter(result),
            [&projection](auto &index) -> synapse_traits::synapse_parameters<SynapseType> *
            { return &(*projection)[index].params_; });
    }
    return result;
}


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
    const core::messaging::SpikeMessage &msg,
    std::vector<StdpProjection<synapse_traits::DeltaSynapse> *> &working_projections,
    knp::core::Population<knp::neuron_traits::SynapticResourceSTDPNeuron<NeuronType>> &population, uint64_t step)
{
    using SynapseType = synapse_traits::STDP<synapse_traits::STDPSynapticResourceRule, synapse_traits::DeltaSynapse>;
    // It's very important that during this function no projection invalidates iterators.
    // Loop over neurons
    for (const auto &spiked_neuron_index : msg.neuron_indexes_)
    {
        auto synapse_params = get_all_connected_synapses<SynapseType>(working_projections, spiked_neuron_index);
        auto &neuron = population[spiked_neuron_index];
        // Calculate neuron ISI status.
        neuron_traits::update_isi<neuron_traits::BLIFATNeuron>(neuron, step);
        if (neuron.isi_status_ == neuron_traits::ISIPeriodType::period_started)
            neuron.stability_ -= neuron.stability_change_at_isi_;

        // This is a new spiking sequence, we can update synapses now.
        if (neuron.isi_status_ != neuron_traits::ISIPeriodType::period_continued)
            for (auto *synapse : synapse_params) synapse->rule_.had_hebbian_update_ = false;

        // Update synapse-only data
        if (neuron.isi_status_ != neuron_traits::ISIPeriodType::is_forced)
        {
            for (auto *synapse : synapse_params)
            {
                // Unconditional decreasing synaptic resource.
                // TODO: NOT HERE! Shouldn't matter now as d_u_ is zero for our task, but the logic is wrong.
                synapse->rule_.synaptic_resource_ -= synapse->rule_.d_u_;
                neuron.free_synaptic_resource_ += synapse->rule_.d_u_;
                // Hebbian plasticity.
                // 1. check if synapse ever got a spike in the current ISI period

                if (is_point_in_interval(
                        neuron.first_isi_spike_ - neuron.isi_max_, step, synapse->rule_.last_spike_step_) &&
                    !synapse->rule_.had_hebbian_update_)
                {
                    // 2. If it did then update synaptic resource value.
                    const float d_h = neuron.d_h_ * std::min(static_cast<float>(std::pow(2, -neuron.stability_)), 1.f);
                    synapse->rule_.synaptic_resource_ += d_h;
                    neuron.free_synaptic_resource_ -= d_h;
                }
            }
        }
        // Recalculating synapse weights. Sometimes it probably doesn't need to happen, check it later.
        recalculate_synapse_weights<knp::synapse_traits::DeltaSynapse>(synapse_params);
    }
}


/**
 * @brief If a neuron's resource is greater than 1 or -1 it should be distributed among all synapses.
 * @tparam NeuronType type of base neuron. BLIFAT for SynapticResourceSTDPBlifat.
 * @param working_projections a list of STDP projections. Right now only supports DeltaSynapse.
 * @param population reference to population.
 * @param step current step.
 */
template <class NeuronType>
void renormalize_resource(
    std::vector<StdpProjection<synapse_traits::DeltaSynapse> *> &working_projections,
    knp::core::Population<knp::neuron_traits::SynapticResourceSTDPNeuron<NeuronType>> &population, uint64_t step)
{
    using SynapseType =
        knp::synapse_traits::STDP<knp::synapse_traits::STDPSynapticResourceRule, synapse_traits::DeltaSynapse>;
    for (size_t neuron_index = 0; neuron_index < population.size(); ++neuron_index)
    {
        auto &neuron = population[neuron_index];
        if (step - neuron.last_step_ <= neuron.isi_max_ &&
            neuron.isi_status_ != neuron_traits::ISIPeriodType::is_forced)
            continue;  // neuron is still in ISI period, skip it.

        if (abs(neuron.free_synaptic_resource_) < neuron.synaptic_resource_threshold_) continue;

        auto synapse_params = get_all_connected_synapses<SynapseType>(working_projections, neuron_index);

        // Divide free resource between all synapses.
        auto add_resource_value =
            neuron.free_synaptic_resource_ / (synapse_params.size() + neuron.resource_drain_coefficient_);

        for (auto synapse : synapse_params) synapse->rule_.synaptic_resource_ += add_resource_value;

        neuron.free_synaptic_resource_ = 0.0F;
        recalculate_synapse_weights(synapse_params);
    }
}


template <class NeuronType>
void do_dopamine_plasticity(
    std::vector<StdpProjection<synapse_traits::DeltaSynapse> *> &working_projections,
    knp::core::Population<knp::neuron_traits::SynapticResourceSTDPNeuron<NeuronType>> &population, uint64_t step)
{
    using SynapseType =
        knp::synapse_traits::STDP<knp::synapse_traits::STDPSynapticResourceRule, synapse_traits::DeltaSynapse>;
    using SynapseParamType = knp::synapse_traits::synapse_parameters<SynapseType>;
    for (size_t neuron_index = 0; neuron_index < population.size(); ++neuron_index)
    {
        auto &neuron = population[neuron_index];
        // Dopamine processing. Dopamine punishment if forced does nothing.
        if (neuron.dopamine_value_ > 0.0 ||
            (neuron.dopamine_value_ < 0.0 && neuron.isi_status_ != neuron_traits::ISIPeriodType::is_forced))
        {
            std::vector<SynapseParamType *> synapse_params =
                get_all_connected_synapses<SynapseType>(working_projections, neuron_index);
            // Change synapse values for both D > 0 and D < 0
            for (auto synapse : synapse_params)
            {
                if (step - synapse->rule_.last_spike_step_ < synapse->rule_.dopamine_plasticity_period_)
                {
                    // Change synapse resource
                    float d_r = neuron.dopamine_value_ *
                                std::min(static_cast<float>(std::pow(2, -neuron.stability_)), 1.F) / 1000.0F;
                    synapse->rule_.synaptic_resource_ += d_r;
                    neuron.free_synaptic_resource_ -= d_r;
                }
            }
            // Stability changes
            if (neuron.is_being_forced_ || neuron.dopamine_value_ < 0)  // TODO check
                // A dopamine reward when forced or a dopamine punishment reduce stability by r*D
                neuron.stability_ -= neuron.dopamine_value_ * neuron.stability_change_parameter_;
            else
            {
                // A dopamine reward when non-forced changes stability by D max(2 - |t(TSS) - ISImax| / ISImax, -1)
                const double dopamine_constant = 2.0;
                const double difference = step - neuron.first_isi_spike_ - neuron.isi_max_;
                neuron.stability_ += neuron.stability_change_parameter_ * neuron.dopamine_value_ *
                                     std::max(dopamine_constant - abs(difference) / neuron.isi_max_, -1.0);
            }
            recalculate_synapse_weights(synapse_params);
        }
    }
}

}  // namespace knp::backends::cpu
