/**
 * @file delta.h
 * @brief Delta synapse type traits.
 * @author Andrey V.
 * @date 26.01.2023
 */

#pragma once

#include <cinttypes>
#include <numeric>

#include "output_types.h"
#include "type_traits.h"

/**
 * @brief Namespace for synapse traits.
 */
namespace knp::synapse_traits
{
/**
 * @brief Delta synapse. Use as a template parameter only.
 */
struct DeltaSynapse;


/**
 * @brief Structure for delta synapse parameters.
 */
template <>
struct synapse_parameters<DeltaSynapse>
{
    /**
     * @brief Default constructor.
     */
    synapse_parameters() : weight_(0.0F), delay_(1), output_type_(knp::synapse_traits::OutputType::EXCITATORY) {}

    /**
     * @brief Constructor.
     * @param weight synaptic weight.
     * @param delay synaptic delay (number of steps).
     * @param type impact type.
     * @note The minimum `delay` value is `1`.
     */
    synapse_parameters(float weight, uint32_t delay, knp::synapse_traits::OutputType type)
        : weight_(weight), delay_(delay), output_type_(type)
    {
    }

    /**
     * @brief Synaptic weight.
     */
    float weight_;

    /**
     * @brief Synaptic delay. Delay of `N` means that a spike sent on step `X` will be received on step `X + N`.
     */
    uint32_t delay_;

    /**
     * @brief Synapse type. Various types have different influences on neuron parameters, as defined by neuron
     * function.
     */
    knp::synapse_traits::OutputType output_type_;
};

}  // namespace knp::synapse_traits
