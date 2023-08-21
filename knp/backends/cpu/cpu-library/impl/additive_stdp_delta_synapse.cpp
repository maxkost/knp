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
        knp::synapse_traits::STDP<knp::synapse_traits::STDPAdditiveRule, knp::synapse_traits::DeltaSynapse>>
        &synapse_params)
{
    return synapse_params.synapse_;
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
}

}  // namespace knp::backends::cpu
