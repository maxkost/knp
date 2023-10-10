/**
 * @file stdp.cpp
 * @brief STDP calculation routines implementation.
 * @author Artiom N.
 * @date 21.08.2023
 */

#include <knp/backends/cpu-library/blifat_population.h>
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
    return synapse_params;
}


template <class ProjectionType>
std::vector<ProjectionType *> find_projections_by_type_and_post_uid()
{
}


/**
 * @brief Apply STDP to all presynaptic connections of a single population.
 * @tparam NeuronType A type of neuron that is compatible with STDP.
 * @param msg spikes emited by population.
 * @param projections All projections. The ones that are not connected, are locked or are of a wrong type are skipped.
 * @param population population.
 * @param step current network step.
 * @param post_synaptic
 * @note all projections are supposed to be of the same type.
 */
template <class NeuronType, class WeightedSynapseType>
void process_spiking_neurons(
    const SpikeMessage &msg, std::vector<core::AllProjectionsVariant> &projections,
    knp::core::Population<knp::neuron_traits::SynapticResourceSTDPNeuron<NeuronType>> &population, uint32_t step,
    bool post_synaptic)
{
    // It's very important that during this function no projection invalidates iterators.
    using ProjectionType = knp::core::Projection<
        knp::synapse_traits::STDP<knp::synapse_traits::STDPSynapticResourceRule, WeightedSynapseType>>;
    using SynapseParamType = knp::synapse_traits::synapse_parameters<
        knp::synapse_traits::STDP<knp::synapse_traits::STDPSynapticResourceRule, WeightedSynapseType>>;

    constexpr size_t projection_type_index = boost::mp11::mp_find<core::AllProjections, ProjectionType>();
    std::vector<ProjectionType *> working_projections;
    for (auto &proj_variant : projections)
    {
        if (proj_variant.index() != projection_type_index) continue;
        auto &projection = std::get<ProjectionType>(proj_variant);
        if (projection.is_locked() || projection.get_postsynaptic() != population.get_uid()) continue;

        working_projections.push_back(&projection);
    }

    for (const auto &spiked_neuron_index : msg.neuron_indexes_)
    {
        auto &neuron = population[spiked_neuron_index];
        if (post_synaptic) neuron_traits::update_isi<neuron_traits::BLIFATNeuron>(neuron, step);

        if (neuron.isi_status_ == neuron_traits::ISIPeriodType::period_continued) continue;

        std::vector<SynapseParamType *> synapse_params;
        for (auto &proj : working_projections)
            for (const auto synaptic_index : proj->get_by_postsynaptic_neuron(spiked_neuron_index))
                synapse_params.push_back(&(*proj)[synaptic_index]);


        for (auto synapse_ptr : synapse_params)
        {
            auto &synapse = *synapse_ptr;
            // Unconditional decreasing synaptic resource.
            synapse.params_.rule_.synaptic_resource_ -= synapse.params_.rule_.d_u_;
            neuron.free_synaptic_resource_ += synapse.params_.rule_.d_u_;
            // Hebbian plasticity.
            const float d_h = neuron.d_h_ * std::min(static_cast<float>(std::pow(2, -neuron.stability_)), 1.f);
            synapse.params_.rule_.synaptic_resource_ += d_h;
            neuron.free_synaptic_resource_ -= d_h;

            // Weight recalculation.
            if (synapse.params_.rule_.synaptic_resource_ >= neuron.synaptic_resource_threshold_)
            {
                const auto syn_w = std::max(synapse.params_.rule_.synaptic_resource_, 0.f);
                const auto weight_diff = synapse.params_.rule_.w_max_ - synapse.params_.rule_.w_min_;
                synapse.params_.weight_ = synapse.params_.rule_.w_min_ + weight_diff * syn_w / (weight_diff + syn_w);
            }
        }

        // Free synaptic resource renormalization.
        if (neuron_traits::ISIPeriodType::not_in_isi == neuron.isi_status_ &&
            neuron.free_synaptic_resource_ > neuron.free_synaptic_resource_threshold_)
        {
            auto add_resource_value =
                neuron.free_synaptic_resource_ / (synapse_params.size() + neuron.resource_drain_coefficient_);
            for (auto synapse : synapse_params)
            {
                synapse->params_.rule_.synaptic_resource_ += add_resource_value;
            }
            neuron.free_synaptic_resource_ = 0.0F;
        }
    }
}

// template<class NeuronType>
// void calculate_synaptic_resource_stdp_delta_synapse_projection(
//     knp::core::Projection<knp::synapse_traits::SynapticResourceSTDPDeltaSynapse> &projection,
//     knp::core::Population<knp::neuron_traits::SynapticResourceSTDPNeuron<NeuronType>> &population,
//     knp::core::MessageEndpoint &endpoint, MessageQueue &future_messages, size_t step_n)
//{
//     SPDLOG_DEBUG("Calculating Synaptic Resource STDP Delta synapse projection");
//
//     using ProjectionType = typename std::decay_t<decltype(projection)>;
//     using ProcessingType = typename ProjectionType::SharedSynapseParameters::ProcessingType;
//
//     auto &shared_params = projection.get_shared_parameters();
//     const auto &stdp_pops = shared_params.stdp_populations_;
//
//     const auto all_messages = endpoint.unload_messages<SpikeMessage>(projection.get_uid());
//
//     // Spike messages to process as usual.
//     std::vector<SpikeMessage> usual_spike_messages;
//     std::vector<SpikeMessage> stdp_only_messages;
//
//     usual_spike_messages.reserve(all_messages.size());
//     stdp_only_messages.reserve(all_messages.size());
//
//     // TODO: Remove cycles.
//     for (const auto &msg : all_messages)
//     {
//         const auto &stdp_pop_iter = stdp_pops.find(msg.header_.sender_uid_);
//
//         if (stdp_pop_iter == stdp_pops.end())
//         {
//             SPDLOG_TRACE("Not STDP population {}", std::string(msg.header_.sender_uid_));
//
//             usual_spike_messages.push_back(msg);
//
//             process_spiking_neurons(msg, projection, population, step_n, false);
//         }
//         else
//         {
//             const auto &[uid, processing_type] = *stdp_pop_iter;
//             assert(uid == msg.header_.sender_uid_);
//
//             if (ProcessingType::STDPAndSpike == processing_type)
//             {
//                 usual_spike_messages.push_back(msg);
//             }
//
//             process_spiking_neurons(msg, projection, population, step_n, true);
//         }
//     }
//
//     if (!usual_spike_messages.empty())
//     {
//         SPDLOG_DEBUG("Calculating STDP Delta synapse projection spikes");
//
//         auto out_iter = calculate_delta_synapse_projection_data(
//             projection, usual_spike_messages, future_messages, step_n, get_delta_synapse_params);
//         if (out_iter != future_messages.end())
//         {
//             SPDLOG_TRACE("Projection is sending an impact message");
//             // Send a message and remove it from the queue.
//             endpoint.send_message(out_iter->second);
//             future_messages.erase(out_iter);
//         }
//     }
//
//     if (!stdp_only_messages.empty())
//         SPDLOG_TRACE("STDP message processing...");
// }

}  // namespace knp::backends::cpu
