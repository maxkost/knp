/**
 * @file save_network.h
 * @brief saving network to Sonata format file.
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

}  // namespace knp::framework::sonata


#define PUT_NEURON_TO_DATASET(pop, param, group)                                                                \
    {                                                                                                           \
        std::vector<decltype(pop.begin()->param)> data;                                                         \
        data.reserve(pop.size());                                                                               \
        std::transform(                                                                                         \
            pop.begin(), pop.end(), std::back_inserter(data), [](const auto &neuron) { return neuron.param; }); \
        group.createDataSet(#param, data);                                                                      \
    }                                                                                                           \
    static_assert(true, "")
