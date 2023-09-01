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

#include "delta_synapse_projection_impl.h"


namespace knp::backends::cpu
{

using knp::core::UID;
using knp::core::messaging::SpikeMessage;

void calculate_projection_part(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection,
    const std::unordered_map<size_t, size_t> &message_in_data, MessageQueue &future_messages, u_int64_t step_n,
    size_t part_start, size_t part_size, std::mutex &mutex)
{
    size_t part_end = std::min(part_start + part_size, projection.size());
    std::vector<std::pair<uint64_t, knp::core::messaging::SynapticImpact>> container;
    for (size_t synapse_index = part_start; synapse_index < part_end; ++synapse_index)
    {
        auto iter = message_in_data.find(synapse_index);
        if (iter == message_in_data.end()) continue;

        const auto &synapse = projection[synapse_index];
        // Add new impact
        // the message is sent on step N - 1, received on N.
        uint64_t key = synapse.params_.delay_ + step_n - 1;

        knp::core::messaging::SynapticImpact impact{
            synapse_index, synapse.params_.weight_, synapse.params_.output_type_, synapse.id_from_, synapse.id_to_};

        container.emplace_back(std::pair{key, impact});
    }
    // Add impacts to future messages queue, it is a shared resource.
    std::lock_guard lock_guard(mutex);
    const auto &projection_uid = projection.get_uid();
    const auto &presynaptic_uid = projection.get_presynaptic();
    const auto &postsynaptic_uid = projection.get_postsynaptic();
    for (auto value : container)
    {
        auto iter = future_messages.find(value.first);
        if (iter != future_messages.end())
            iter->second.impacts_.push_back(value.second);
        else
        {
            knp::core::messaging::SynapticImpactMessage message_out{
                {projection_uid, step_n}, postsynaptic_uid, presynaptic_uid, {value.second}};
            future_messages.insert(std::make_pair(value.first, message_out));
        }
    }
}


std::unordered_map<uint64_t, size_t> convert_spikes(const SpikeMessage &message)
{
    std::unordered_map<size_t, size_t> result;
    for (auto n : message.neuron_indexes_)
    {
        auto iter = result.find(n);
        if (result.end() == iter)
            result.insert({n, 1});
        else
            ++(iter->second);
    }
    return result;
}


void calculate_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection, knp::core::MessageEndpoint &endpoint,
    MessageQueue &future_messages, size_t step_n)
{
    SPDLOG_DEBUG("Calculating Delta synapse projection");

    auto out_iter = calculate_delta_synapse_projection_data(
        projection, endpoint.unload_messages<SpikeMessage>(projection.get_uid()), future_messages, step_n);
    if (out_iter != future_messages.end())
    {
        SPDLOG_TRACE("Projection is sending an impact message");
        // Send a message and remove it from the queue.
        endpoint.send_message(out_iter->second);
        future_messages.erase(out_iter);
    }
}

}  // namespace knp::backends::cpu
