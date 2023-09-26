/**
 * @file stdp.cpp
 * @brief STDP calculation routines implementation.
 * @author Artiom N.
 * @date 21.08.2023
 */

#include <knp/backends/cpu-library/delta_synapse_projection.h>

#include <spdlog/spdlog.h>

#include <unordered_map>
#include <utility>
#include <vector>

#include "delta_synapse_projection_impl.h"


namespace knp::backends::cpu
{

using knp::core::UID;
using knp::core::messaging::SpikeMessage;

enum class ISIPeriodType
{
    not_in_the_isi,
    period_started,
    period_continued
};


struct NeuronCharacteristics
{
    ISIPeriodType type_;
    uint32_t last_step_;
};


const knp::synapse_traits::synapse_parameters<knp::synapse_traits::DeltaSynapse> &get_delta_synapse_params(
    const knp::synapse_traits::synapse_parameters<
        knp::synapse_traits::STDP<knp::synapse_traits::STDPSynapticResourceRule, knp::synapse_traits::DeltaSynapse>>
        &synapse_params)
{
    return synapse_params.synapse_;
}


void process_spiking_neurons(
    const SpikeMessage &msg, knp::core::Projection<knp::synapse_traits::SynapticResourceSTDPDeltaSynapse> &projection,
    std::unordered_map<uint32_t, NeuronCharacteristics> &neuron_caracteristics, uint32_t step, bool post_synaptic)
{
    auto &shared_params = projection.get_shared_parameters();
    auto &synapses_parameters = shared_params.synapses_parameters_;

    for (const auto &spiked_neuron_index : msg.neuron_indexes_)
    {
        auto &neuron_data = neuron_caracteristics[spiked_neuron_index];

        for (const auto &synapse_index : projection.get_by_presynaptic_neuron(spiked_neuron_index))
        {
            auto &synapse = projection[synapse_index];
            // Unconditional decreasing synaptic resource.
            synapse.params_.rule_.synaptic_resource_ -= synapse.params_.rule_.d_u_;
            synapses_parameters.free_synaptic_resource_ += synapse.params_.rule_.d_u_;
            // Hebbian plasticity value reset.
            if (synapses_parameters.d_h_ >= 0 && ISIPeriodType::period_started == neuron_data.type_)
                synapses_parameters.d_h_ = synapses_parameters.d_h_initial_;

            // Hebbian plasticity.
            const float d_h = synapses_parameters.d_h_ *
                              std::min(static_cast<float>(std::pow(2, -synapses_parameters.stability_)), 1.f);
            synapse.params_.rule_.synaptic_resource_ += d_h;
            synapses_parameters.free_synaptic_resource_ -= d_h;

            // Weight recalculation.
            if (synapse.params_.rule_.synaptic_resource_ >= synapses_parameters.synaptic_resource_threshold_)
            {
                const auto syn_w = std::max(synapse.params_.rule_.synaptic_resource_, 0.f);
                const auto weight_diff = synapse.params_.rule_.w_max_ - synapse.params_.rule_.w_min_;
                synapse.params_.synapse_.weight_ =
                    synapse.params_.rule_.w_min_ + weight_diff * syn_w / (weight_diff + syn_w);
            }

            // Free synaptic resource renormalization.
        }

        if (post_synaptic)
        {
            switch (neuron_data.type_)
            {
                case ISIPeriodType::not_in_the_isi:
                    if (neuron_data.last_step_ - step < synapses_parameters.isi_max_)
                        neuron_data.type_ = ISIPeriodType::period_started;
                    break;
                case ISIPeriodType::period_started:
                    if (neuron_data.last_step_ - step < synapses_parameters.isi_max_)
                        neuron_data.type_ = ISIPeriodType::period_continued;
                    else
                        neuron_data.type_ = ISIPeriodType::not_in_the_isi;
                    break;
                case ISIPeriodType::period_continued:
                    if (neuron_data.last_step_ - step >= synapses_parameters.isi_max_)
                        neuron_data.type_ = ISIPeriodType::not_in_the_isi;
                    break;
            }
        }
    }
}


void calculate_synaptic_resource_stdp_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::SynapticResourceSTDPDeltaSynapse> &projection,
    knp::core::MessageEndpoint &endpoint, MessageQueue &future_messages, size_t step_n)
{
    SPDLOG_DEBUG("Calculating Synaptic Resource STDP Delta synapse projection");

    std::unordered_map<uint32_t, NeuronCharacteristics> neuron_caracteristics;

    using ProjectionType = typename std::decay_t<decltype(projection)>;
    using ProcessingType = typename ProjectionType::SharedSynapseParameters::ProcessingType;

    auto &shared_params = projection.get_shared_parameters();
    const auto &stdp_pops = shared_params.stdp_populations_;

    const auto all_messages = endpoint.unload_messages<SpikeMessage>(projection.get_uid());
    // Spike messages to process as usual.
    std::vector<SpikeMessage> usual_spike_messages;
    std::vector<SpikeMessage> stdp_only_messages;

    usual_spike_messages.reserve(all_messages.size());
    stdp_only_messages.reserve(all_messages.size());

    // TODO: Remove cycles.
    for (const auto &msg : all_messages)
    {
        const auto &stdp_pop_iter = stdp_pops.find(msg.header_.sender_uid_);

        if (stdp_pop_iter == stdp_pops.end())
        {
            SPDLOG_TRACE("Not STDP population {}", std::string(msg.header_.sender_uid_));

            usual_spike_messages.push_back(msg);

            process_spiking_neurons(msg, projection, neuron_caracteristics, step_n, false);
        }
        else
        {
            const auto &[uid, processing_type] = *stdp_pop_iter;
            assert(uid == msg.header_.sender_uid_);

            if (ProcessingType::STDPAndSpike == processing_type)
            {
                usual_spike_messages.push_back(msg);
            }

            process_spiking_neurons(msg, projection, neuron_caracteristics, step_n, true);
        }
    }

    SPDLOG_DEBUG("{}", shared_params.synapses_parameters_.isi_max_);

    if (!usual_spike_messages.empty())
    {
        SPDLOG_DEBUG("Calculating STDP Delta synapse projection spikes");

        auto out_iter = calculate_delta_synapse_projection_data(
            projection, usual_spike_messages, future_messages, step_n, get_delta_synapse_params);
        if (out_iter != future_messages.end())
        {
            SPDLOG_TRACE("Projection is sending an impact message");
            // Send a message and remove it from the queue.
            endpoint.send_message(out_iter->second);
            future_messages.erase(out_iter);
        }
    }

    if (!stdp_only_messages.empty())
    {
        SPDLOG_TRACE("STDP message processing...");
    }

    //    for (auto &s : projection)
    //    {
    //        SPDLOG_TRACE("Applying STDP rule...");
    //        auto &rule = s.params_.rule_;
    //        const auto period = rule.tau_plus_ + rule.tau_minus_;

    //        if (rule.presynaptic_spike_times_.size() >= period && rule.postsynaptic_spike_times_.size() >= period)
    //        {
    //            STDPFormula stdp_formula(rule.tau_plus_, rule.tau_minus_, 1, 1);
    //            SPDLOG_TRACE("Old weight = {}", s.params_.synapse_.weight_);
    //            s.params_.synapse_.weight_ += stdp_formula(rule.presynaptic_spike_times_,
    //            rule.postsynaptic_spike_times_);
    //            SPDLOG_TRACE("New weight = {}", s.params_.synapse_.weight_);
    //            rule.presynaptic_spike_times_.clear();
    //            rule.postsynaptic_spike_times_.clear();
    //        }
    //    }
}

}  // namespace knp::backends::cpu
