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
struct neuron_parameters<SynapticResourceSTDPNeuron<NeuronType>>
{
    neuron_parameters<NeuronType> neuron_;

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
     * @brief Synaptic resource threshold value.
     */
    // cppcheck-suppress unusedStructMember
    float synaptic_resource_threshold_;
};

}  // namespace knp::neuron_traits
