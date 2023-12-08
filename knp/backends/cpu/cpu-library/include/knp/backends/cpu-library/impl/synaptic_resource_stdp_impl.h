/**
 * @file synaptic_resource_stdp_impl.h
 * @brief Synaptic resource based STDP neurons adapter.
 * @author Artiom N.
 * @date 06.10.2023
 */

#pragma once
#include <knp/backends/cpu-library/impl/base_stdp_impl.h>
#include <knp/core/messaging/spike_message.h>
#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/core/uid.h>
#include <knp/synapse-traits/stdp_common.h>
#include <knp/synapse-traits/stdp_synaptic_resource_rule.h>
#include <knp/synapse-traits/stdp_type_traits.h>

#include <algorithm>
#include <limits>
#include <numeric>
#include <utility>
#include <vector>

#include <boost/mp11.hpp>
/**
 * @brief Namespace for neuron traits.
 */
namespace knp::backends::cpu
{
bool is_neuroplastic_population(const core::AllPopulationsVariant &population)
{
    using PopulationType =
        knp::core::Population<knp::neuron_traits::SynapticResourceSTDPNeuron<knp::neuron_traits::BLIFATNeuron>>;
    constexpr size_t expected_index = boost::mp11::mp_find<core::AllPopulations, PopulationType>();
    size_t index = population.index();
    return index == expected_index;
}


template <class SynapseType, class ProjectionContainer>
std::vector<knp::core::Projection<SynapseType> *> find_projection_by_type_and_postsynaptic(
    ProjectionContainer &projections, const knp::core::UID &post_uid, bool exclude_locked)
{
    using ProjectionType = knp::core::Projection<SynapseType>;
    std::vector<knp::core::Projection<SynapseType> *> result;
    constexpr auto type_index = boost::mp11::mp_find<synapse_traits::AllSynapses, SynapseType>();
    for (auto &projection : projections)
    {
        if (projection.arg_.index() != type_index) continue;
        ProjectionType *projection_ptr = &(std::get<type_index>(projection.arg_));
        if (projection_ptr->is_locked() && exclude_locked) continue;

        if (projection_ptr->get_postsynaptic() == post_uid) result.push_back(projection_ptr);
    }
    return result;
}


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
            [&projection](auto const &index) -> synapse_traits::synapse_parameters<SynapseType> *
            { return &(*projection)[index].params_; });
    }
    return result;
}


/**
 * @brief Update spike sequence state for the neuron. It's called after a neuron sends a spike.
 * @tparam NeuronType base neuron type.
 * @param neuron neuron parameters.
 * @param step current step.
 * @return new state.
 */
template <class NeuronType>
neuron_traits::ISIPeriodType update_isi(
    neuron_traits::neuron_parameters<neuron_traits::SynapticResourceSTDPNeuron<NeuronType>> &neuron, uint64_t step)
{
    if (neuron.is_being_forced_)  // This neuron got a forcing spike this turn and doesn't continue its spiking sequence
    {
        neuron.isi_status_ = neuron_traits::ISIPeriodType::is_forced;
        // Do not update last_step_
        return neuron.isi_status_;
    }
    else
    {
        switch (neuron.isi_status_)
        {
            case neuron_traits::ISIPeriodType::is_forced:
                neuron.isi_status_ = neuron_traits::ISIPeriodType::period_started;
                neuron.first_isi_spike_ = step;
                break;
            case neuron_traits::ISIPeriodType::period_started:
                if (neuron.last_step_ - step < neuron.isi_max_)
                    neuron.isi_status_ = neuron_traits::ISIPeriodType::period_continued;
                break;
            case neuron_traits::ISIPeriodType::period_continued:
                if (neuron.last_step_ - step >= neuron.isi_max_ || neuron.dopamine_value_ != 0)
                {
                    neuron.isi_status_ = neuron_traits::ISIPeriodType::period_started;
                    neuron.first_isi_spike_ = step;
                }
                break;
            case neuron_traits::ISIPeriodType::not_in_period:
                neuron.isi_status_ = neuron_traits::ISIPeriodType::period_started;
                neuron.first_isi_spike_ = step;
                break;
            default:
                throw std::runtime_error("Not supported ISI status.");
        }
    }
    neuron.last_step_ = step;
    return neuron.isi_status_;
}


/**
 * @brief Apply STDP to all presynaptic connections of a single population.
 * @tparam NeuronType A type of neuron that is compatible with STDP.
 * @param msg spikes emited by population.
 * @param working_projections All projections. The ones that are not connected, are locked or are of a wrong type are
 * skipped.
 * @param population population.
 * @param step current network step.
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
        update_isi<neuron_traits::BLIFATNeuron>(neuron, step);
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
            if (neuron.is_being_forced_ || neuron.dopamine_value_ < 0)
            {
                // A dopamine reward when forced or a dopamine punishment reduce stability by r*D
                neuron.stability_ -= neuron.dopamine_value_ * neuron.stability_change_parameter_;
                neuron.stability_ = std::max(neuron.stability_, 0.0F);
            }
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


template <class DeltaLikeSynapse>
struct WeightUpdateSTDP<synapse_traits::STDP<synapse_traits::STDPSynapticResourceRule, DeltaLikeSynapse>>
{
    using Synapse = synapse_traits::STDP<synapse_traits::STDPSynapticResourceRule, DeltaLikeSynapse>;
    static void init_projection(
        knp::core::Projection<Synapse> &projection, std::vector<core::messaging::SpikeMessage> &messages, uint64_t step)
    {
    }

    static void init_synapse(knp::synapse_traits::synapse_parameters<Synapse> &params, uint64_t step)
    {
        params.rule_.last_spike_step_ = step;
    }

    static void modify_weights(knp::core::Projection<Synapse> &projection) {}
};


template <class NeuronType, class SynapseType>
void do_STDP_resource_plasticity(
    knp::core::Population<knp::neuron_traits::SynapticResourceSTDPNeuron<NeuronType>> &population,
    std::vector<knp::core::Projection<synapse_traits::STDP<synapse_traits::STDPSynapticResourceRule, SynapseType>> *>
        working_projections,
    const std::optional<core::messaging::SpikeMessage> &message, uint64_t step)
{
    // Call learning functions on all found projections:
    // 1. If neurons generated spikes, process these neurons.
    if (message.has_value())
        knp::backends::cpu::process_spiking_neurons<neuron_traits::BLIFATNeuron>(
            message.value(), working_projections, population, step);

    // 2. Do dopamine plasticity.
    knp::backends::cpu::do_dopamine_plasticity(working_projections, population, step);

    // 3. Renormalize resources if needed.
    knp::backends::cpu::renormalize_resource(working_projections, population, step);
}
}  // namespace knp::backends::cpu
