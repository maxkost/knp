/**
 * @file altai_lif_neuron.cpp
 * @brief AltaiLIF neuron procedures.
 * @kaspersky_support An. Vartenkov
 * @date 15.05.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
    // cppcheck-suppress constParameterReference
    HighFive::File &file_h5, const core::Population<knp::neuron_traits::AltAILIF> &population)
{
    SPDLOG_TRACE("Adding population {} to HDF5...", std::string(population.get_uid()));
    throw std::runtime_error("AltAILIF neuron saving unimplemented.");
}


template <>
core::Population<neuron_traits::AltAILIF> load_population<neuron_traits::AltAILIF>(
    const HighFive::Group &nodes_group, const std::string &population_name)
{
    SPDLOG_DEBUG("Loading nodes...");
    throw std::runtime_error("AltAILIF neuron loading unimplemented.");
}


}  // namespace knp::framework::sonata
