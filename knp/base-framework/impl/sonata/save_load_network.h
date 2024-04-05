/**
 * @file save_load_network.cpp
 * @brief saving and loading network to Sonata format file.
 * @author An. Vartenkov
 * @date 28.03.2024
 */

#pragma once
#include <knp/core/population.h>
#include <knp/core/projection.h>

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

// Read parameter values for both projections and populations
template <class Attr>
std::vector<Attr> read_parameter(
    const HighFive::Group &population_group, const std::string &param_name, size_t pop_size, const Attr &default_value)
{
    std::vector<Attr> result(pop_size);
    try
    {
        auto dataset = population_group.getDataSet(param_name);
        dataset.read(result);
    }
    catch (std::exception &exc)
    {
        return std::vector(pop_size, default_value);
    }
    return result;
}


template <class Neuron>
core::Population<Neuron> load_population(const HighFive::Group &nodes_group, const std::string &population_name)
{
    throw std::logic_error("Trying to load an unimplemented population type");
}


template <class Synapse>
core::Projection<Synapse> load_projection(const HighFive::Group &edges_group, const std::string &projection_name)
{
    throw std::logic_error("Trying to load an unimplemented projection type");
}


}  // namespace knp::framework

#define LOAD_NEURONS_PARAMETER(target, neuron_type, parameter, h5_group, pop_size)                                 \
    {                                                                                                              \
        const auto values =                                                                                        \
            read_parameter(h5_group, #parameter, pop_size, neuron_traits::default_values<neuron_type>::parameter); \
        for (size_t i = 0; i < target.size(); ++i) target[i].parameter = values[i];                                \
    }                                                                                                              \
    static_assert(true, "")


#define LOAD_SYNAPSE_PARAMETER(target, synapse_type, parameter, h5_group, proj_size)                                  \
    {                                                                                                                 \
        const auto values =                                                                                           \
            read_parameter(h5_group, #parameter, proj_size, synapse_traits::default_values<synapse_type>::parameter); \
        for (size_t i = 0; i < target.size(); ++i) target[i].parameter = values[i];                                   \
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
