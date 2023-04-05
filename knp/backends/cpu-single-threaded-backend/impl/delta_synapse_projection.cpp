/**
 * @file delta_synapse_projection.cpp
 * @brief DeltaSynapse calculation routines implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */

#include "delta_synapse_projection.h"

#include <unordered_map>
#include <utility>


void calculate_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection, knp::core::MessageEndpoint &endpoint,
    MessageQueue &future_messages, size_t step_n)
{
    // TODO: Get message
    const knp::core::messaging::SpikeMessage message_in{{knp::core::UID{}, 0}, {}};

    // TODO: Get time
    time_t time = 0;
    for (auto &neuron_index : message_in.neuron_indexes_)
    {
        auto synapses = projection.get_by_presynaptic_neuron(neuron_index);
        // Add new impacts
        for (const auto &synapse_index : synapses)
        {
            auto &syn = projection[synapse_index];
            size_t key = syn.params.delay_ + step_n;
            knp::core::messaging::SynapticImpact impact{synapse_index, syn.params.weight_, syn.id_from, syn.id_to};

            auto iter = future_messages.find(key);
            if (iter == future_messages.end())
            {
                knp::core::messaging::SynapticImpactMessage message_out{
                    {projection.get_uid(), time},
                    projection.get_postsynaptic(),
                    projection.get_presynaptic(),
                    projection.get_output_type(),
                    {impact}};
                future_messages.insert(std::make_pair(key, message_out));
            }
            else
            {
                (*iter).second.impacts_.push_back(impact);
            }
        }
    }
    auto out_iter = future_messages.find(step_n);
    if (out_iter != future_messages.end())
    {
        // Send message and remove it from queue
        endpoint.send_message(out_iter->second);
        future_messages.erase(out_iter);
    }
}
