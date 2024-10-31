/**
 * @file additive_delta_synapse.cpp
 * @brief Functions for loading and saving Additive STDP delta synapses.
 * @kaspersky_support Artiom N.
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

#include <knp/core/projection.h>
#include <knp/synapse-traits/delta.h>
#include <knp/synapse-traits/stdp_synaptic_resource_rule.h>

#include <filesystem>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>

#include "../csv_content.h"
#include "../highfive.h"
#include "../load_network.h"
#include "../save_network.h"
#include "type_id_defines.h"


namespace knp::framework::sonata
{
namespace fs = std::filesystem;
using AdditiveDeltaSynapse = knp::synapse_traits::AdditiveSTDPDeltaSynapse;


template <>
std::string get_synapse_type_name<AdditiveDeltaSynapse>()
{
    return "knp:AdditiveSTDPDeltaSynapse";
}


template <>
void add_projection_to_h5<core::Projection<AdditiveDeltaSynapse>>(
    // cppcheck-suppress constParameterReference
    HighFive::File &file_h5, const knp::core::Projection<AdditiveDeltaSynapse> &projection)
{
    throw std::runtime_error("AdditiveDeltaSynapse saving unimplemented.");
}


template <>
core::Projection<AdditiveDeltaSynapse> load_projection(
    const HighFive::Group &edges_group, const std::string &projection_name)
{
    throw std::runtime_error("AdditiveDeltaSynapse loading unimplemented.");
}

}  // namespace knp::framework::sonata
