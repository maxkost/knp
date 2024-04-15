/**
 * @file load_network.h
 * @brief loading network to Sonata format file.
 * @author An. Vartenkov
 * @date 15.04.2024
 */

#pragma once

#include <knp/core/population.h>
#include <knp/core/projection.h>

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include "highfive.h"


namespace knp::framework::sonata
{
namespace fs = std::filesystem;

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

}  // namespace knp::framework::sonata


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
