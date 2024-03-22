//
// Created by an_vartenkov on 22.03.24.
//
#pragma once
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <exception>
#include <string>


namespace knp::framework
{
template <class Synapse>
int get_synapse_type_id()
{
    throw std::runtime_error("Unknown synapse type id");
}


template <>
int get_synapse_type_id<synapse_traits::DeltaSynapse>()
{
    return 1000;
}  // TODO: Need a specific file for that


template <class Synapse>
std::string get_synapse_type_name()
{
    throw std::runtime_error("Undefined synapse name");
}


template <>
std::string get_synapse_type_name<synapse_traits::DeltaSynapse>()
{
    return "knp:BasicDeltaSynapse";
}

template <class Neuron>
int get_neuron_type_id()
{
    throw std::runtime_error("Unknown neuron type id");
}

template <>
int get_neuron_type_id<neuron_traits::BLIFATNeuron>()
{
    return 1000;
}  // TODO: Need a specific file for that

struct VirtualNeuron;

template <>
int get_neuron_type_id<VirtualNeuron>()
{
    return 0;
}

template <class Synapse>
std::string get_neuron_type_name()
{
    throw std::runtime_error("Undefined neuron name");
}

template <>
std::string get_neuron_type_name<neuron_traits::BLIFATNeuron>()
{
    return "knp:BasicBlifatNeuron";
}

}  // namespace knp::framework
