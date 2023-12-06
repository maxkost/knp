/**
 * @file base_stdp.h
 * @brief Common templates used for different STDP variants.
 * @author A. Vartenkov
 * @date 04.11.2023
 */
#pragma once
#include <knp/core/messaging/messaging.h>
#include <knp/core/projection.h>

#include <vector>

/**
 * @brief Namespace for CPU backends.
 */
namespace knp::backends::cpu
{

template <class DeltaLikeSynapse>
struct WeightUpdateSTDP
{
    static void init_projection(
        knp::core::Projection<DeltaLikeSynapse> &projection, std::vector<core::messaging::SpikeMessage> &messages,
        uint64_t step)
    {
    }

    static void init_synapse(knp::synapse_traits::synapse_parameters<DeltaLikeSynapse> &projection, uint64_t step) {}

    static void modify_weights(knp::core::Projection<DeltaLikeSynapse> &projection) {}
};

}  // namespace knp::backends::cpu
