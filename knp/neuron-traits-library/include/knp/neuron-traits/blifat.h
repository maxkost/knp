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

#include <boost/mpl/at.hpp>
#include <boost/mpl/vector.hpp>


namespace knp::neuron_traits
{

struct BLIFATNeuron;

template <>
struct neuron_parameters<BLIFATNeuron>
{
    constexpr parameters = boost::hana::make_map(hana::make_pair("membrane_potential"_s, hana::type_c<float>));
};

}  // namespace knp::neuron_traits
