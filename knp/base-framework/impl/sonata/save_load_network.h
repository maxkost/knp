//
// Created by an_vartenkov on 28.03.24.
//
#pragma once

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

namespace knp::framework
{
namespace fs = std::filesystem;

template <class Projection>
void add_projection_to_h5(HighFive::File &, const Projection &)
{
    throw std::runtime_error("Couldn't save projection of unknown class");
}


template <class Population>
void add_population_to_h5(HighFive::File &, const Population &)
{
    throw std::runtime_error("Saving model: unknown population type");
}


template <class Synapse>
void add_synapse_type_to_csv(const fs::path &)
{
    throw std::logic_error("Unknown synapse type");
}


template <class Neuron>
void add_neuron_type_to_csv(const fs::path &)
{
    throw std::logic_error("Unknown neuron type");
}


template <class Neuron>
core::Population<Neuron> load_population(const HighFive::Group &nodes_group, const std::string &population_name);

}  // namespace knp::framework

#define LOAD_NEURONS_ATTRIBUTE(target, neuron_type, attribute, h5_group, pop_size)                                 \
    {                                                                                                              \
        const auto values =                                                                                        \
            read_attribute(h5_group, #attribute, pop_size, neuron_traits::default_values<neuron_type>::attribute); \
        for (size_t i = 0; i < target.size(); ++i) target[i].attribute = values[i];                                \
    }                                                                                                              \
    static_assert(true, "")


#define LOAD_SYNAPSE_ATTRIBUTE(target, synapse_type, attribute, h5_group, proj_size)                                  \
    {                                                                                                                 \
        const auto values =                                                                                           \
            read_attribute(h5_group, #attribute, proj_size, synapse_traits::default_values<synapse_type>::attribute); \
        for (size_t i = 0; i < target.size(); ++i) target[i].attribute = values[i];                                   \
    }                                                                                                                 \
    static_assert(true, "")


#define PUT_NEURON_TO_DATASET(pop, param, group)                                                                \
    {                                                                                                           \
        std::vector<decltype(pop.begin()->param)> data;                                                         \
        data.reserve(pop.size());                                                                               \
        std::transform(                                                                                         \
            pop.begin(), pop.end(), std::back_inserter(data), [](const auto &neuron) { return neuron.param; }); \
        group.createDataSet(#param, data);                                                                      \
    }                                                                                                           \
    static_assert(true, "")
