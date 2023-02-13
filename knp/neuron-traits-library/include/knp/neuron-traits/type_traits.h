/**
 * @file type_traits.h
 * @brief Common neuron types traits.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once
#include<type_traits>
namespace knp::neuron_traits
{

template <typename NeuronType>
struct neuron_parameters;

template<typename NeuronType>
struct default_values;


}  // namespace knp::neuron_traits
