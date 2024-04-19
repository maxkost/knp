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
 * @brief Delta synapse. 
 * @note Use as a template parameter only.
 */
struct DeltaSynapse;


/**
 * @brief Default values for Delta synapse parameters.
 */
template <>
struct default_values<DeltaSynapse>
{
    /**
     * @brief Synaptic weight default value.
     */
    constexpr static float weight_ = 0.0F;

    /**
     * @brief Synaptic delay default value. 
     * @note Value of `1` is the least delay possible.
     */
    constexpr static uint32_t delay_ = 1;

    /**
     * @brief Synapse type default value.
     */
    constexpr static OutputType output_type_ = OutputType::EXCITATORY;
};


/**
 * @brief Structure for delta synapse parameters.
 */
template <>
struct synapse_parameters<DeltaSynapse>
{
    /**
     * @brief Default constructor.
     */
    synapse_parameters()
        : weight_(default_values<DeltaSynapse>::weight_),
          delay_(default_values<DeltaSynapse>::delay_),
          output_type_(default_values<DeltaSynapse>::output_type_)
    {
    }

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
