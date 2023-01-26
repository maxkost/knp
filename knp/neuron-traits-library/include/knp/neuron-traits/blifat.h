/**
 * @file blifat.h
 * @brief BLIFAT neuron types traits.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include <string>
#include <tuple>

#include <boost/mpl/at.hpp>
#include <boost/mpl/vector.hpp>


namespace knp::neuron_traits
{

struct BLIFATNeuron;

template <>
struct neuron_parameters<BLIFATNeuron>
{
    const std::tuple<std::string> parameter_names{"membrane_potential"};
    using ParametersTypes = boost::mpl::vector<float>;
};

}  // namespace knp::neuron_traits
