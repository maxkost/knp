//
// Created by an_vartenkov on 27.03.24.
//
#include "type_id_defines.h"

#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

namespace knp::framework
{
template <>
int get_synapse_type_id<synapse_traits::DeltaSynapse>()
{
    return 1000;
}  // TODO: Need a specific file for that

template <>
std::string get_synapse_type_name<synapse_traits::DeltaSynapse>()
{
    return "knp:BasicDeltaSynapse";
}

template <>
int get_neuron_type_id<neuron_traits::BLIFATNeuron>()
{
    return 1000;
}  // TODO: Need a specific file for that


template <>
std::string get_neuron_type_name<neuron_traits::BLIFATNeuron>()
{
    return "knp:BasicBlifatNeuron";
}


template <>
int get_neuron_type_id<VirtualNeuron>()
{
    return 0;
}
}  // namespace knp::framework
