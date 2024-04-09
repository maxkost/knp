//
// Created by an_vartenkov on 22.03.24.
//
#pragma once

#include <stdexcept>
#include <string>


namespace knp::framework::sonata
{
template <class Synapse>
int get_synapse_type_id()
{
    throw std::runtime_error("Unknown synapse type id");
}


template <class Synapse>
std::string get_synapse_type_name()
{
    throw std::runtime_error("Undefined synapse name");
}

template <class Neuron>
int get_neuron_type_id()
{
    throw std::runtime_error("Unknown neuron type id");
}

struct VirtualNeuron;

template <class Synapse>
std::string get_neuron_type_name()
{
    throw std::runtime_error("Undefined neuron name");
}

}  // namespace knp::framework::sonata
