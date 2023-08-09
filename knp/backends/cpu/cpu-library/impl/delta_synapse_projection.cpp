/**
 * @file delta_synapse_projection.cpp
 * @brief DeltaSynapse and type index calculation routines implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include <knp/backends/cpu-library/delta_synapse_projection.h>

#include <spdlog/spdlog.h>

#include <unordered_map>
#include <utility>
#include <vector>


using knp::core::UID;
using knp::core::messaging::SpikeMessage;


MessageQueue::const_iterator calculate_delta_synapse_projection_data(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection, knp::core::MessageEndpoint &endpoint,
    MessageQueue &future_messages, size_t step_n)
{
    SPDLOG_TRACE("Calculating Delta synapse projection data");
    std::vector<SpikeMessage> messages = endpoint.unload_messages<SpikeMessage>(projection.get_uid());
    SpikeMessage message_in = messages.empty() ? SpikeMessage{{UID{}, 0}, {}} : messages[0];

    const auto projection_uid = projection.get_uid();
    const auto projection_presynaptic = projection.get_presynaptic();
    const auto projection_postsynaptic = projection.get_postsynaptic();

    for (const auto &neuron_index : message_in.neuron_indexes_)
    {
        auto synapses = projection.get_by_presynaptic_neuron(neuron_index);
        // Add new impacts
        for (const auto &synapse_index : synapses)
        {
            auto &syn = projection[synapse_index];

            // the message is sent on step N - 1, received on N.
            size_t key = syn.params.delay_ + step_n - 1;

            knp::core::messaging::SynapticImpact impact{
                synapse_index, syn.params.weight_, syn.params.output_type_, syn.id_from, syn.id_to};

            auto iter = future_messages.find(key);
            if (iter != future_messages.end())
            {
                iter->second.impacts_.push_back(impact);
            }
            else
            {
                knp::core::messaging::SynapticImpactMessage message_out{
                    {projection_uid, step_n}, projection_postsynaptic, projection_presynaptic, {impact}};
                future_messages.insert(std::make_pair(key, message_out));
            }
        }
    }
    return future_messages.find(step_n);
}


void calculate_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection, knp::core::MessageEndpoint &endpoint,
    MessageQueue &future_messages, size_t step_n)
{
    SPDLOG_DEBUG("Calculating Delta synapse projection");
    auto out_iter = calculate_delta_synapse_projection_data(projection, endpoint, future_messages, step_n);
    if (out_iter != future_messages.end())
    {
        SPDLOG_TRACE("Projection is sending an impact message");
        // Send a message and remove it from the queue
        endpoint.send_message(out_iter->second);
        future_messages.erase(out_iter);
    }
}


void calculate_additive_stdp_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::AdditiveSTDPDeltaSynapse> &projection,
    knp::core::MessageEndpoint &endpoint, MessageQueue &future_messages, size_t step_n)
{
    SPDLOG_DEBUG("Calculating Delta synapse projection");
}


void calculate_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection, knp::core::MessageEndpoint &endpoint,
    MessageQueue &future_messages, size_t step_n, std::mutex &m)
{
    auto out_iter = calculate_delta_synapse_projection_data(projection, endpoint, future_messages, step_n);
    if (out_iter != future_messages.end())
    {
        std::lock_guard<std::mutex> lg(m);
        endpoint.send_message(out_iter->second);
        future_messages.erase(out_iter);
    }
}
