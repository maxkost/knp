/**
 * @file delta_synapse_projection.cpp
 * @brief DeltaSynapse and type index calculation routines implementation.
 * @author Artiom N.
 * @date 21.03.2023
 */

#include <knp/backends/cpu-library/delta_synapse_projection.h>

#include <spdlog/spdlog.h>

#include <unordered_map>
#include <utility>
#include <vector>


using knp::core::UID;
using knp::core::messaging::SpikeMessage;


// TODO: Split it up into reusable code fragments
void calculate_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection, knp::core::MessageEndpoint &endpoint,
    MessageQueue &future_messages, size_t step_n)
{
    SPDLOG_TRACE("Calculating delta synapse projection");
    std::vector<SpikeMessage> messages = endpoint.unload_messages<SpikeMessage>(projection.get_uid());
    SpikeMessage message_in = messages.empty() ? SpikeMessage{{UID{}, 0}, {}} : messages[0];

    uint64_t time = step_n;
    for (auto &neuron_index : message_in.neuron_indexes_)
    {
        auto synapses = projection.get_by_presynaptic_neuron(neuron_index);
        // Add new impacts
        for (const auto &synapse_index : synapses)
        {
            auto &syn = projection[synapse_index];

            // the message is sent on step N-1, received on N.
            size_t key = syn.params.delay_ + step_n - 1;

            knp::core::messaging::SynapticImpact impact{
                synapse_index, syn.params.weight_, syn.params.output_type_, syn.id_from, syn.id_to};

            auto iter = future_messages.find(key);
            if (iter == future_messages.end())
            {
                knp::core::messaging::SynapticImpactMessage message_out{
                    {projection.get_uid(), time},
                    projection.get_postsynaptic(),
                    projection.get_presynaptic(),
                    {impact}};
                future_messages.insert(std::make_pair(key, message_out));
            }
            else
            {
                iter->second.impacts_.push_back(impact);
            }
        }
    }
    auto out_iter = future_messages.find(step_n);
    if (out_iter != future_messages.end())
    {
        SPDLOG_TRACE("Projection is sending an impact message");
        // Send a message and remove it from the queue
        endpoint.send_message(out_iter->second);
        future_messages.erase(out_iter);
    }
}
