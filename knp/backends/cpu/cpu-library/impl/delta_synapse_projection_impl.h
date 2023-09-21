/**
 * @file delta_synapse_projection_impl.h
 * @brief DeltaSynapse and type index calculation routines implementation header.
 * @author Artiom N.
 * @date 21.08.2023
 */

#pragma once

#include <knp/backends/cpu-library/delta_synapse_projection.h>

#include <spdlog/spdlog.h>

#include <unordered_map>
#include <utility>
#include <vector>


namespace knp::backends::cpu
{

using knp::core::UID;
using knp::core::messaging::SpikeMessage;


template <typename ProjectionType>
MessageQueue::const_iterator calculate_delta_synapse_projection_data(
    ProjectionType &projection, const std::vector<SpikeMessage> &messages, MessageQueue &future_messages, size_t step_n,
    std::function<knp::synapse_traits::synapse_parameters<knp::synapse_traits::DeltaSynapse>(
        const typename ProjectionType::SynapseParameters &)>
        sp_getter = [](const typename ProjectionType::SynapseParameters &sp) { return sp; })
{
    SPDLOG_TRACE("Calculating Delta synapse projection data");

    for (const auto &message : messages)
    {
        const auto &message_data = message.neuron_indexes_;
        for (const auto &spiked_neuron_index : message_data)
        {
            auto synapses = projection.get_by_presynaptic_neuron(spiked_neuron_index);
            for (auto synapse_index : synapses)
            {
                auto &synapse = projection[synapse_index];
                const auto &synapse_params = sp_getter(synapse.params_);
                // the message is sent on step N - 1, received on N.
                size_t future_step = synapse_params.delay_ + step_n - 1;
                knp::core::messaging::SynapticImpact impact{
                    synapse_index, synapse_params.weight_, synapse_params.output_type_, synapse.id_from_,
                    synapse.id_to_};

                auto iter = future_messages.find(future_step);
                if (iter != future_messages.end())
                    iter->second.impacts_.push_back(impact);
                else
                {
                    knp::core::messaging::SynapticImpactMessage message_out{
                        {projection.get_uid(), step_n},
                        projection.get_postsynaptic(),
                        projection.get_presynaptic(),
                        {impact}};
                    future_messages.insert(std::make_pair(future_step, message_out));
                }
            }
        }
    }

    return future_messages.find(step_n);
}

}  // namespace knp::backends::cpu
