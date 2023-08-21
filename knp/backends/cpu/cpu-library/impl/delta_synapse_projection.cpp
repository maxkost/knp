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


const knp::synapse_traits::synapse_parameters<knp::synapse_traits::DeltaSynapse> &get_delta_synapse_params(
    const knp::synapse_traits::synapse_parameters<knp::synapse_traits::DeltaSynapse> &synapse_params)
{
    return synapse_params;
}


const knp::synapse_traits::synapse_parameters<knp::synapse_traits::DeltaSynapse> &get_delta_synapse_params(
    const knp::synapse_traits::synapse_parameters<
        knp::synapse_traits::STDP<knp::synapse_traits::STDPAdditiveRule, knp::synapse_traits::DeltaSynapse>>
        &synapse_params)
{
    return synapse_params.synapse_;
}


template <typename ProjectionType>
MessageQueue::const_iterator calculate_delta_synapse_projection_data(
    ProjectionType &projection, const std::vector<SpikeMessage> &messages, MessageQueue &future_messages, size_t step_n)
{
    SPDLOG_TRACE("Calculating Delta synapse projection data");

    if (messages.empty() || messages[0].neuron_indexes_.empty()) return future_messages.find(step_n);
    auto message_data = convert_spikes(messages[0]);

    for (size_t synapse_index = 0; synapse_index < projection.size(); ++synapse_index)
    {
        const auto &synapse = projection[synapse_index];
        auto spike_iter = message_data.find(synapse.id_from_);
        if (spike_iter == message_data.end()) continue;

        const auto &synapse_params = get_delta_synapse_params(synapse.params_);
        // the message is sent on step N - 1, received on N.
        size_t future_step = synapse_params.delay_ + step_n - 1;
        knp::core::messaging::SynapticImpact impact{
            synapse_index, synapse_params.weight_, synapse_params.output_type_, synapse.id_from_, synapse.id_to_};

        auto iter = future_messages.find(future_step);
        if (iter != future_messages.end())
            iter->second.impacts_.push_back(impact);
        else
        {
            knp::core::messaging::SynapticImpactMessage message_out{
                {projection.get_uid(), step_n}, projection.get_postsynaptic(), projection.get_presynaptic(), {impact}};
            future_messages.insert(std::make_pair(future_step, message_out));
        }
    }

    return future_messages.find(step_n);
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


float stdp_w(float a_plus, float a_minus, float tau_plus, float tau_minus, float x)
{
    // Zhang et al. 1998.
    return x > 0 ? a_plus * std::exp(-x / tau_plus) : a_minus * std::exp(x / tau_minus);
}


float stdp_delta_w(
    const std::vector<size_t> &presynaptic_spikes, const std::vector<size_t> &postsynaptic_spikes,
    std::function<float(float)> w)
{
    // Gerstner and al. 1996, Kempter et al. 1999.

    assert(presynaptic_spikes.size() == postsynaptic_spikes.size());

    float w_j = 0;

    for (const auto &t_f : presynaptic_spikes)
        for (const auto &t_n : postsynaptic_spikes)
        {
            // cppcheck-suppress useStlAlgorithm
            w_j += w(t_n - t_f);
        }

    return w_j;
}


void calculate_additive_stdp_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::AdditiveSTDPDeltaSynapse> &projection,
    knp::core::MessageEndpoint &endpoint, MessageQueue &future_messages, size_t step_n)
{
    SPDLOG_DEBUG("Calculating Delta synapse projection");

    using ProjectionType = typename std::decay_t<decltype(projection)>;
    using ProcessingType =
        typename ProjectionType::SynapseSpecificParameters<ProjectionType::ProjectionSynapseType>::ProcessingType;

    const auto &stdp_pops = projection.get_common_paratemeters().stdp_populations_;

    const auto all_messages = endpoint.unload_messages<SpikeMessage>(projection.get_uid());
    // Spike messages to process as usual.
    std::vector<SpikeMessage> usual_spike_messages;
    std::vector<SpikeMessage> stdp_only_messages;

    usual_spike_messages.reserve(all_messages.size());
    stdp_only_messages.reserve(all_messages.size());

    for (const auto &msg : all_messages)
    {
        const auto &stdp_pop_iter = stdp_pops.find(msg.header_.sender_uid_);
        if (stdp_pop_iter == stdp_pops.end())
        {
            SPDLOG_TRACE("Not STDP population {}", std::string(msg.header_.sender_uid_));
            usual_spike_messages.push_back(msg);
        }
        else
        {
            const auto &[uid, processing_type] = *stdp_pop_iter;
            assert(uid == msg.header_.sender_uid_);

            if (ProcessingType::STDPAndSpike == processing_type)
            {
                SPDLOG_TRACE("STDP synapse and spike");
                usual_spike_messages.push_back(msg);
            }
            else if (ProcessingType::STDPOnly == processing_type)
            {
                SPDLOG_TRACE("STDP synapse");
            }
            else
            {
                SPDLOG_ERROR(
                    "Incorrect processing type {} for population {}", static_cast<int>(processing_type),
                    std::string(uid));
            }
        }
    }

    if (!usual_spike_messages.empty())
    {
        SPDLOG_DEBUG("Calculating STDP Delta synapse projection spikes");

        auto out_iter =
            calculate_delta_synapse_projection_data(projection, usual_spike_messages, future_messages, step_n);
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
        void;
    }
}

}  // namespace knp::backends::cpu
