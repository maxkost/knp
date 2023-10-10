/**
 * @file synaptic_resource_stdp.h
 * @brief Synaptci resource based STDP neurons adapter.
 * @author Artiom N.
 * @date 06.10.2023
 */

#pragma once

#include <limits>
#include <numeric>
#include <utility>

#include <boost/mp11.hpp>

#include "type_traits.h"

/**
 * @brief Namespace for neuron traits.
 */
namespace knp::neuron_traits
{

template <typename NeuronType>
struct SynapticResourceSTDPNeuron;

enum class ISIPeriodType
{
    not_in_isi,
    period_started,
    period_continued
};


/**
 * @brief Template for the neurons, which are supported synaptic resource based STDP.
 */
template <typename NeuronType>
struct default_values<SynapticResourceSTDPNeuron<NeuronType>>
{
    default_values<NeuronType> neuron_;
};

/**
 * @brief Template for the neurons, which are supported synaptic resource based STDP.
 */
template <typename NeuronType>
struct neuron_parameters<SynapticResourceSTDPNeuron<NeuronType>> : public neuron_parameters<NeuronType>
{
    /**
     * @brief Free synaptic resource.
     */
    // cppcheck-suppress unusedStructMember
    float free_synaptic_resource_ = 1;
    /**
     * @brief Free synaptic resource threshold value.
     */
    // cppcheck-suppress unusedStructMember
    float free_synaptic_resource_threshold_ = -1;
    /**
     * @brief Synaptic resource is divided by (number of synapses + resource drain coefficient).
     */
    // cppcheck-suppress unusedStructMember
    uint32_t resource_drain_coefficient_ = 0;
    /**
     * @brief Stability.
     */
    // cppcheck-suppress unusedStructMember
    float stability_;
    /**
     * @brief Time between spikes in the ISI period.
     */
    // cppcheck-suppress unusedStructMember
    uint32_t isi_max_;
    /**
     * @brief Hebbian plasticity value.
     */
    // cppcheck-suppress unusedStructMember
    float d_h_;
    /**
     * @brief Hebbian plasticity initial value.
     */
    // cppcheck-suppress unusedStructMember
    float d_h_initial_;
    /**
     * @brief Synaptic resource threshold value. Isn't used
     */
    // cppcheck-suppress unusedStructMember
    float synaptic_resource_threshold_;

    /**
     * @brief ISI period status.
     */
    ISIPeriodType isi_status_;

    /**
     * @brief TODO: I have no idea what we need this for.
     */
    // cppcheck-suppress unusedStructMember
    uint32_t last_step_;
};


template <class NeuronType>
ISIPeriodType update_isi(neuron_parameters<SynapticResourceSTDPNeuron<NeuronType>> &neuron, uint64_t step)
{
    switch (neuron.isi_status_)
    {
        case neuron_traits::ISIPeriodType::period_started:
            if (neuron.last_step_ - step < neuron.isi_max_)
                neuron.isi_status_ = neuron_traits::ISIPeriodType::period_continued;
            break;
        case neuron_traits::ISIPeriodType::period_continued:
            if (neuron.last_step_ - step >= neuron.isi_max_)
                neuron.isi_status_ = neuron_traits::ISIPeriodType::period_started;
            break;
        default:
            throw std::runtime_error("Not supported ISI status.");
    }
    neuron.last_step_ = step;
}

}  // namespace knp::neuron_traits
