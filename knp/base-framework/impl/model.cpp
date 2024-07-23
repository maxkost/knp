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
    // TODO: check if projection_uid in network.projections.
    in_channels_.insert(decltype(in_channels_)::value_type(channel_uid, projection_uid));
}


void Model::add_output_channel(const core::UID &channel_uid, const core::UID &population_uid)
{
    // TODO: check if population_uid in network.populations.
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

}  // namespace knp::framework
