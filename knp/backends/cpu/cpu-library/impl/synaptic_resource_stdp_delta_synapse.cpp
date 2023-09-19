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


const knp::synapse_traits::synapse_parameters<knp::synapse_traits::DeltaSynapse> &get_delta_synapse_params(
    const knp::synapse_traits::synapse_parameters<
        knp::synapse_traits::STDP<knp::synapse_traits::STDPSynapticResourceRule, knp::synapse_traits::DeltaSynapse>>
        &synapse_params)
{
    return synapse_params.synapse_;
}


void calculate_synaptic_resource_stdp_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::SynapticResourceSTDPDeltaSynapse> &projection,
    knp::core::MessageEndpoint &endpoint, MessageQueue &future_messages, size_t step_n)
{
    SPDLOG_DEBUG("Calculating Synaptic Resource STDP Delta synapse projection");

    //    using ProjectionType = typename std::decay_t<decltype(projection)>;
    //    using ProcessingType = typename ProjectionType::SharedSynapseParameters::ProcessingType;

    //    const auto &stdp_pops = projection.get_shared_parameters().stdp_populations_;

    const auto all_messages = endpoint.unload_messages<SpikeMessage>(projection.get_uid());
    // Spike messages to process as usual.
    std::vector<SpikeMessage> usual_spike_messages;
    std::vector<SpikeMessage> stdp_only_messages;

    usual_spike_messages.reserve(all_messages.size());
    stdp_only_messages.reserve(all_messages.size());

    //    // TODO: Remove cycles.
    //    for (const auto &msg : all_messages)
    //    {
    //        const auto &stdp_pop_iter = stdp_pops.find(msg.header_.sender_uid_);
    //        if (stdp_pop_iter == stdp_pops.end())
    //        {
    //            SPDLOG_TRACE("Not STDP population {}", std::string(msg.header_.sender_uid_));
    //            usual_spike_messages.push_back(msg);
    //        }
    //        else
    //        {
    //            const auto &[uid, processing_type] = *stdp_pop_iter;
    //            assert(uid == msg.header_.sender_uid_);

    //            if (ProcessingType::STDPAndSpike == processing_type)
    //            {
    //                SPDLOG_TRACE("STDP synapse and spike");
    //                usual_spike_messages.push_back(msg);
    //                stdp_only_messages.push_back(msg);
    //            }
    //            else if (ProcessingType::STDPOnly == processing_type)
    //            {
    //                SPDLOG_TRACE("STDP synapse");
    //                stdp_only_messages.push_back(msg);
    //            }
    //            else
    //            {
    //                SPDLOG_ERROR(
    //                    "Incorrect processing type {} for population {}", static_cast<int>(processing_type),
    //                    std::string(uid));
    //            }
    //        }
    //    }

    //    if (!usual_spike_messages.empty())
    //    {
    //        SPDLOG_DEBUG("Calculating STDP Delta synapse projection spikes");

    //        append_spike_times(
    //            projection, usual_spike_messages,
    //            [&projection](uint32_t neuron_index) { return projection.get_by_presynaptic_neuron(neuron_index); },
    //            &knp::synapse_traits::STDPAdditiveRule<knp::synapse_traits::DeltaSynapse>::presynaptic_spike_times_);

    //        auto out_iter = calculate_delta_synapse_projection_data(
    //            projection, usual_spike_messages, future_messages, step_n, get_delta_synapse_params);
    //        if (out_iter != future_messages.end())
    //        {
    //            SPDLOG_TRACE("Projection is sending an impact message");
    //            // Send a message and remove it from the queue.
    //            endpoint.send_message(out_iter->second);
    //            future_messages.erase(out_iter);
    //        }
    //    }

    //    if (!stdp_only_messages.empty())
    //    {
    //        SPDLOG_TRACE("STDP message processing...");
    //        // Fill synapses post-synaptic spike queue.
    //        append_spike_times(
    //            projection, stdp_only_messages,
    //            [&projection](uint32_t neuron_index) { return projection.get_by_postsynaptic_neuron(neuron_index); },
    //            &knp::synapse_traits::STDPAdditiveRule<knp::synapse_traits::DeltaSynapse>::postsynaptic_spike_times_);
    //    }

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
    //            rule.postsynaptic_spike_times_); SPDLOG_TRACE("New weight = {}", s.params_.synapse_.weight_);
    //            rule.presynaptic_spike_times_.clear();
    //            rule.postsynaptic_spike_times_.clear();
    //        }
    //    }
}

}  // namespace knp::backends::cpu
