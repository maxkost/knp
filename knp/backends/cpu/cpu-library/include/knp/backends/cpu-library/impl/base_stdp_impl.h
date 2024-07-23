/**
 * @file base_stdp_impl.h
 * @brief Common templates used for different STDP variants.
 * @author A. Vartenkov
 * @date 04.11.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
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
        const knp::core::Projection<DeltaLikeSynapse> &projection,
        const std::vector<core::messaging::SpikeMessage> &messages, uint64_t step)
    {
    }

    static void init_synapse(const knp::synapse_traits::synapse_parameters<DeltaLikeSynapse> &projection, uint64_t step)
    {
    }

    static void modify_weights(const knp::core::Projection<DeltaLikeSynapse> &projection) {}
};

}  // namespace knp::backends::cpu
