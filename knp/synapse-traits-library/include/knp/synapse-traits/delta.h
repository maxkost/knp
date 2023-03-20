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
    synapse_parameters() : weight_(0.0F), delay_(1) {}
    synapse_parameters(float weight, std::size_t delay) : weight_(weight), delay_(delay) {}
    float weight_;
    std::size_t delay_;
};
}  // namespace knp::synapse_traits
