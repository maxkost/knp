/**
 * @file altai_lif_neuron.cpp
 * @brief AltaiLIF neuron procedures file.
 * @author An. Vartenkov
 * @date 15.05.2024
 */

#include <knp/core/population.h>
#include <knp/neuron-traits/altai_lif.h>

#include <spdlog/spdlog.h>

#include <boost/lexical_cast.hpp>

#include "../csv_content.h"
#include "../highfive.h"
#include "../load_network.h"
#include "../save_network.h"
#include "type_id_defines.h"


namespace knp::framework::sonata
{

template <>
std::string get_neuron_type_name<neuron_traits::AltAILIF>()
{
    return "knp:AltAILIF";
}


template <>
void add_population_to_h5<core::Population<knp::neuron_traits::AltAILIF>>(
    HighFive::File &file_h5, const core::Population<knp::neuron_traits::AltAILIF> &population)
{
    SPDLOG_TRACE("Adding population {} to hdf5", std::string(population.get_uid()));
    throw std::runtime_error("AltAILIF neuron saving unimplemented!");
}


template <>
core::Population<neuron_traits::AltAILIF> load_population<neuron_traits::AltAILIF>(
    const HighFive::Group &nodes_group, const std::string &population_name)
{
    SPDLOG_DEBUG("Loading nodes");
    throw std::runtime_error("AltAILIF neuron loading unimplemented!");
}


}  // namespace knp::framework::sonata
