/**
 * @file delta_synapse_projection_impl.h
 * @brief DeltaSynapse calculation routines definition.
 * @author Artiom N.
 * @date 21.02.2023
 */

#pragma once

#include <knp/core/message_bus.h>
#include <knp/core/projection.h>
#include <knp/synapse-traits/delta.h>

#include <spdlog/spdlog.h>

#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "additive_stdp_impl.h"


/**
 * @brief Namespace for CPU backends.
 */
namespace knp::backends::cpu
{
/**
 * @brief Type of the message queue.
 */
typedef knp::core::messaging::SynapticMessageQueue MessageQueue;

/**
 * @brief Convert spike vector to unordered map.
 * @param message spike message.
 * @return unordered map of `{index : number of instances}`. Number of instances usually equals `1`.
 */
std::unordered_map<uint64_t, size_t> convert_spikes(const knp::core::messaging::SpikeMessage &message);


template <class DeltaLikeSynapse>
void calculate_projection_part_impl(
    knp::core::Projection<DeltaLikeSynapse> &projection, const std::unordered_map<size_t, size_t> &message_in_data,
    MessageQueue &future_messages, u_int64_t step_n, size_t part_start, size_t part_size, std::mutex &mutex);


template <class DeltaLikeSynapse>
void calculate_delta_synapse_projection_impl(
    knp::core::Projection<DeltaLikeSynapse> &projection, knp::core::MessageEndpoint &endpoint,
    MessageQueue &future_messages, size_t step_n);


template <class ProjectionType>
constexpr bool is_forcing()
{
    return false;
}


template <>
constexpr bool is_forcing<knp::core::Projection<synapse_traits::DeltaSynapse>>()
{
    return true;
}


template <typename ProjectionType>
MessageQueue::const_iterator calculate_delta_synapse_projection_data(
    ProjectionType &projection, std::vector<core::messaging::SpikeMessage> &messages, MessageQueue &future_messages,
    size_t step_n,
    std::function<knp::synapse_traits::synapse_parameters<knp::synapse_traits::DeltaSynapse>(
        const typename ProjectionType::SynapseParameters &)>
        sp_getter = [](const typename ProjectionType::SynapseParameters &sp) { return sp; })
{
    SPDLOG_TRACE("Calculating Delta synapse projection data");
    using SynapseType = typename ProjectionType::ProjectionSynapseType;
    WeightUpdateSTDP<SynapseType>::init_projection(projection, messages, step_n);

    for (const auto &message : messages)
    {
        const auto &message_data = message.neuron_indexes_;
        for (const auto &spiked_neuron_index : message_data)
        {
            auto synapses = projection.get_by_presynaptic_neuron(spiked_neuron_index);
            for (auto synapse_index : synapses)
            {
                auto &synapse = projection[synapse_index];
                WeightUpdateSTDP<SynapseType>::init_synapse(synapse.params_, step_n);
                const auto &synapse_params = sp_getter(synapse.params_);
                // the message is sent on step N - 1, received on N.
                size_t future_step = synapse_params.delay_ + step_n - 1;
                knp::core::messaging::SynapticImpact impact{
                    synapse_index, synapse_params.weight_, synapse_params.output_type_,
                    static_cast<uint32_t>(synapse.id_from_), static_cast<uint32_t>(synapse.id_to_)};

                auto iter = future_messages.find(future_step);
                if (iter != future_messages.end())
                    iter->second.impacts_.push_back(impact);
                else
                {
                    knp::core::messaging::SynapticImpactMessage message_out{
                        {projection.get_uid(), step_n},
                        projection.get_postsynaptic(),
                        projection.get_presynaptic(),
                        is_forcing<ProjectionType>(),
                        {impact}};
                    future_messages.insert(std::make_pair(future_step, message_out));
                }
            }
        }
    }
    WeightUpdateSTDP<SynapseType>::modify_weights(projection);
    return future_messages.find(step_n);
}


template <class DeltaLikeSynapse>
void calculate_projection_part_impl(
    knp::core::Projection<DeltaLikeSynapse> &projection, const std::unordered_map<size_t, size_t> &message_in_data,
    MessageQueue &future_messages, u_int64_t step_n, size_t part_start, size_t part_size, std::mutex &mutex)
{
    size_t part_end = std::min(part_start + part_size, projection.size());
    std::vector<std::pair<uint64_t, knp::core::messaging::SynapticImpact>> container;
    for (size_t synapse_index = part_start; synapse_index < part_end; ++synapse_index)
    {
        auto &synapse = projection[synapse_index];
        // update_step(synapse.params_, step_n);
        // TODO: Move update logic here too.
        auto iter = message_in_data.find(synapse.id_from_);
        if (iter == message_in_data.end()) continue;

        // Add new impact
        // the message is sent on step N - 1, received on N.
        uint64_t key = synapse.params_.delay_ + step_n - 1;
        knp::core::messaging::SynapticImpact impact{
            synapse_index, synapse.params_.weight_ * iter->second, synapse.params_.output_type_,
            static_cast<uint32_t>(synapse.id_from_), static_cast<uint32_t>(synapse.id_to_)};

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
                {projection_uid, step_n},
                postsynaptic_uid,
                presynaptic_uid,
                is_forcing<core::Projection<DeltaLikeSynapse>>(),
                {value.second}};
            future_messages.insert(std::make_pair(value.first, message_out));
        }
    }
}


std::unordered_map<uint64_t, size_t> convert_spikes(const core::messaging::SpikeMessage &message)
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


template <class DeltaLikeSynapseType>
void calculate_delta_synapse_projection_impl(
    knp::core::Projection<DeltaLikeSynapseType> &projection, knp::core::MessageEndpoint &endpoint,
    MessageQueue &future_messages, size_t step_n)
{
    SPDLOG_DEBUG("Calculating Delta synapse projection");

    auto messages = endpoint.unload_messages<core::messaging::SpikeMessage>(projection.get_uid());
    auto out_iter = calculate_delta_synapse_projection_data(projection, messages, future_messages, step_n);
    if (out_iter != future_messages.end())
    {
        SPDLOG_TRACE("Projection is sending an impact message");
        // Send a message and remove it from the queue.
        endpoint.send_message(out_iter->second);
        future_messages.erase(out_iter);
    }
}

}  // namespace knp::backends::cpu
