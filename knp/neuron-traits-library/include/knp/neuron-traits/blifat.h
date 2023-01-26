/**
 * @file blifat.h
 * @brief BLIFAT neuron types traits.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include <string>
#include <tuple>
#include <utility>

#include "type_traits.h"


namespace knp::neuron_traits
{

struct BLIFATNeuron;

template <>
struct neuron_parameters<BLIFATNeuron>
{
    float membrane_potential_;
};

}  // namespace knp::neuron_traits
