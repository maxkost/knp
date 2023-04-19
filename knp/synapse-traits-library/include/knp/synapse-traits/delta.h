/**
 * @file delta.h
 * @brief Delta synapse types traits.
 * @author Andrey V.
 * @date 26.01.2023
 */

#pragma once

#include <numeric>

#include "type_traits.h"


namespace knp::synapse_traits
{
struct DeltaSynapse;

template <>
struct synapse_parameters<DeltaSynapse>
{
    synapse_parameters() : weight_(0.0F), delay_(1), output_type_(knp::synapse_traits::OutputType::EXCITATORY) {}

    synapse_parameters(float weight, uint32_t delay, knp::synapse_traits::OutputType type)
        : weight_(weight), delay_(delay), output_type_(type)
    {
    }

    float weight_;
    uint32_t delay_;
    knp::synapse_traits::OutputType output_type_;
};

}  // namespace knp::synapse_traits
