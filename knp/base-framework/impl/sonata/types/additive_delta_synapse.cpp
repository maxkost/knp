/**
 * @file additive_delta_synapse.cpp
 * @brief Functions for loading and saving Additive STDP delta synapses.
 * @author Artiom N.
 * @date 15.05.2024
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
    HighFive::File &file_h5, const knp::core::Projection<AdditiveDeltaSynapse> &projection)
{
    throw std::runtime_error("AdditiveDeltaSynapse add unimplemented!");
}


template <>
core::Projection<AdditiveDeltaSynapse> load_projection(
    const HighFive::Group &edges_group, const std::string &projection_name)
{
    throw std::runtime_error("AdditiveDeltaSynapse load unimplemented!");
}

}  // namespace knp::framework::sonata
