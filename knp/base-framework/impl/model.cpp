/**
 * @file model.cpp
 * @brief Model class implementation.
 * @author Artiom N.
 * @date 30.03.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/framework/model.h>

#include <spdlog/spdlog.h>


namespace knp::framework
{

void Model::add_input_channel(const core::UID &channel_uid, const core::UID &projection_uid)
{
    if (!network_.is_projection_exists(projection_uid))
    {
        throw std::logic_error("Projection with UID = " + std::string(projection_uid) + " doesn't exist.");
    }
    in_channels_.insert(decltype(in_channels_)::value_type(channel_uid, projection_uid));
}


void Model::add_output_channel(const core::UID &channel_uid, const core::UID &population_uid)
{
    if (!network_.is_population_exists(population_uid))
    {
        throw std::logic_error("Population with UID = " + std::string(population_uid) + " doesn't exist.");
    }

    out_channels_.insert(decltype(out_channels_)::value_type(channel_uid, population_uid));
}


const std::unordered_multimap<core::UID, core::UID, core::uid_hash> &Model::get_input_channels() const
{
    return in_channels_;
}


const std::unordered_multimap<core::UID, core::UID, core::uid_hash> &Model::get_output_channels() const
{
    return out_channels_;
}


namespace nt = knp::neuron_traits;

#define INSTANCE_POPULATION_FUNCTIONS(n, template_for_instance, neuron_type)      \
    template KNP_DECLSPEC void Model::connect_output_population<nt::neuron_type>( \
        const core::UID &, const core::Population<nt::neuron_type> &);

namespace st = knp::synapse_traits;

#define INSTANCE_PROJECTION_FUNCTIONS(n, template_for_instance, synapse_type)     \
    template KNP_DECLSPEC void Model::connect_input_projection<st::synapse_type>( \
        const core::UID &, const core::Projection<st::synapse_type> &);

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_POPULATION_FUNCTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_NEURONS))
// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_PROJECTION_FUNCTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_SYNAPSES))

}  // namespace knp::framework
